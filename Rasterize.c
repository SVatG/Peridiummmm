/**
 * Rasterizer
 */

#include <stdlib.h>
#include <string.h>

#include "VectorLibrary/VectorFixed.h"
#include "VectorLibrary/MatrixFixed.h"

#include "LED.h"

#define WIDTH 320
#define HEIGHT 200

#define MIN_DIFF 0

#define R(x) (F( ((x) & (7 << 5)) >> 5 ))
#define G(x) (F( ((x) & (7 << 2)) >> 2 ))
#define B(x) (F( (x) & (3) ))

#define RGB(r,g,b) ((r)<<5 | (g)<<2 | (b))

#define V(x,y,z) {F(x),F(y),F(z),F(1)}

#define Viewport(x,w,s) (imul(idiv((x),(w))+IntToFixed(1),IntToFixed((s)/2)))

typedef struct {
	ivec4_t p;
	uint32_t c;
} vertex_t;

typedef struct {
	ivec3_t p;
	uint32_t c;
} ss_vertex_t;

typedef struct {
	ss_vertex_t v[3];
} triangle_t;

typedef struct {
	int32_t v[3];
} index_triangle_t;

#include "rad.h"
#include "spikes.h"

void RasterizeTriangle(uint8_t* image, triangle_t tri, imat4x4_t modelview, imat4x4_t proj ) {
	// Winding test
	if(
		imul(tri.v[1].p.x - tri.v[0].p.x, tri.v[2].p.y - tri.v[0].p.y) -
		imul(tri.v[2].p.x - tri.v[0].p.x, tri.v[1].p.y - tri.v[0].p.y)
		< 0
	) {
		return;
	}

	// Vertex sorting
	ss_vertex_t upperVertex;
	ss_vertex_t centerVertex;
	ss_vertex_t lowerVertex;

	if(tri.v[0].p.y < tri.v[1].p.y) {
		upperVertex = tri.v[0];
		lowerVertex = tri.v[1];
	}
	else {
		upperVertex = tri.v[1];
		lowerVertex = tri.v[0];
	}

	if(tri.v[2].p.y < upperVertex.p.y) {
		centerVertex = upperVertex;
		upperVertex = tri.v[2];
	}
	else {
		if(tri.v[2].p.y > lowerVertex.p.y) {
			centerVertex = lowerVertex;
			lowerVertex = tri.v[2];
		}
		else {
			centerVertex = tri.v[2];
		}
	}

	// scanline counters
	int32_t scanline;
	int32_t scanlineMax;

	// left / right x and deltas
	int32_t leftX;
	int32_t leftXd;
	int32_t rightX;
	int32_t rightXd;

	// left color and color delta
	int32_t leftColR;
	int32_t leftColG;
	int32_t leftColB;
	int32_t leftColRd;
	int32_t leftColGd;
	int32_t leftColBd;

	// color and color x deltas
	int32_t colR;
	int32_t colG;
	int32_t colB;
	int32_t colRdX;
	int32_t colGdX;
	int32_t colBdX;

	// calculate y differences
	int32_t upperDiff = upperVertex.p.y - centerVertex.p.y;
	int32_t lowerDiff = upperVertex.p.y - lowerVertex.p.y;


	// deltas
	int32_t upperCenter;
	int32_t upperLower;

	// check if we have a triangle at all (Special case A)
	if(lowerDiff == 0 && upperDiff == 0) {
		return;
	}

	// calculate whole-triangle deltas
	int32_t temp = idiv(centerVertex.p.y-upperVertex.p.y,lowerVertex.p.y-upperVertex.p.y);
	int32_t width = imul(temp, (lowerVertex.p.x-upperVertex.p.x)) + (upperVertex.p.x-centerVertex.p.x);
	if(width == 0) {
		return;
	}
	colRdX = idiv(imul(temp, (R(lowerVertex.c)-R(upperVertex.c))) + (R(upperVertex.c)-R(centerVertex.c)),width);
	colGdX = idiv(imul(temp, (G(lowerVertex.c)-G(upperVertex.c))) + (G(upperVertex.c)-G(centerVertex.c)),width);
	colBdX = idiv(imul(temp, (B(lowerVertex.c)-B(upperVertex.c))) + (B(upperVertex.c)-B(centerVertex.c)),width);

	// guard against special case B: flat upper edge
	if(upperDiff == 0 ) {

		if(upperVertex.p.x < centerVertex.p.x) {
			leftX = upperVertex.p.x;
			leftColR = IntToFixed(upperVertex.c & (7 << 5))>>5;
			leftColG = IntToFixed(upperVertex.c & (7 << 2))>>2;
			leftColB = IntToFixed(upperVertex.c & (3));
			rightX = centerVertex.p.x;

			leftXd = idiv(upperVertex.p.x - lowerVertex.p.x, lowerDiff);
			rightXd = idiv(centerVertex.p.x - lowerVertex.p.x, lowerDiff);
		}
		else {
			leftX = centerVertex.p.x;
			leftColR = IntToFixed(centerVertex.c & (7 << 5))>>5;
			leftColG = IntToFixed(centerVertex.c & (7 << 2))>>2;
			leftColB = IntToFixed(centerVertex.c & (3));
			rightX = upperVertex.p.x;

			leftXd = idiv(centerVertex.p.x - lowerVertex.p.x, lowerDiff);
			rightXd = idiv(upperVertex.p.x - lowerVertex.p.x, lowerDiff);
		}

		leftColRd = idiv(leftColR - (IntToFixed(lowerVertex.c & (7 << 5))>>5), lowerDiff);
		leftColGd = idiv(leftColG - (IntToFixed(lowerVertex.c & (7 << 2))>>2), lowerDiff);
		leftColBd = idiv(leftColB - (IntToFixed(lowerVertex.c & (3))), lowerDiff);

		goto lower_half_render;
	}

	// calculate deltas
	upperCenter = idiv(upperVertex.p.x - centerVertex.p.x, upperDiff);
	upperLower = idiv(upperVertex.p.x - lowerVertex.p.x, lowerDiff);

	// upper triangle half
	leftX = rightX = upperVertex.p.x;

	leftColR = IntToFixed(upperVertex.c & (7 << 5))>>5;
	leftColG = IntToFixed(upperVertex.c & (7 << 2))>>2;
	leftColB = IntToFixed(upperVertex.c & (3));


	if(upperCenter < upperLower) {
		leftXd = upperCenter;
		rightXd = upperLower;

		leftColRd = idiv(leftColR - (IntToFixed(centerVertex.c & (7 << 5))>>5), upperDiff);
		leftColGd = idiv(leftColG - (IntToFixed(centerVertex.c & (7 << 2))>>2), upperDiff);
		leftColBd = idiv(leftColB - (IntToFixed(centerVertex.c & (3))), upperDiff);
	}
	else {
		leftXd = upperLower;
		rightXd = upperCenter;

		leftColRd = idiv(leftColR - (IntToFixed(lowerVertex.c & (7 << 5))>>5), lowerDiff);
		leftColGd = idiv(leftColG - (IntToFixed(lowerVertex.c & (7 << 2))>>2), lowerDiff);
		leftColBd = idiv(leftColB - (IntToFixed(lowerVertex.c & (3))), lowerDiff);
	}

	scanlineMax = FixedToRoundedInt(centerVertex.p.y);
	for(scanline = FixedToRoundedInt(upperVertex.p.y); scanline < scanlineMax; scanline++ ) {
		int32_t xMax = FixedToRoundedInt(rightX);
		for(int32_t x = FixedToRoundedInt(leftX); x <= xMax; x++) {
			image[x+scanline*WIDTH] = (FixedToRoundedInt(colR)<<5) | (FixedToRoundedInt(colG)<<2) | (FixedToRoundedInt(colB));
			colR += colRdX;
			colG += colGdX;
			colB += colBdX;
		}
		leftX += leftXd;
		rightX += rightXd;
		leftColR += leftColRd;
		colR = leftColR;
		leftColG += leftColGd;
		colG = leftColG;
		leftColB += leftColBd;
		colB = leftColB;
	}

	// Guard against special case C: flat lower edge
	int32_t centerDiff = centerVertex.p.y - lowerVertex.p.y;
	if(centerDiff == 0) {
		return;
	}

	// calculate lower triangle half deltas
	if(upperCenter < upperLower) {
		leftX = centerVertex.p.x;
		leftXd = idiv(centerVertex.p.x - lowerVertex.p.x, centerDiff);

		leftColR = IntToFixed(centerVertex.c & (7 << 5))>>5;
		leftColG = IntToFixed(centerVertex.c & (7 << 2))>>2;
		leftColB = IntToFixed(centerVertex.c & (3));

		leftColRd = idiv(leftColR - (IntToFixed(lowerVertex.c & (7 << 5))>>5), centerDiff);
		leftColGd = idiv(leftColG - (IntToFixed(lowerVertex.c & (7 << 2))>>2), centerDiff);
		leftColBd = idiv(leftColB - (IntToFixed(lowerVertex.c & (3))), centerDiff);
	}
	else {
		rightX = centerVertex.p.x;
		rightXd = idiv(centerVertex.p.x - lowerVertex.p.x, centerDiff);
	}

lower_half_render:

	// lower triangle half
	scanlineMax = FixedToRoundedInt(lowerVertex.p.y);

	colR = leftColR;
	colG = leftColG;
	colB = leftColB;
	
	for(scanline = FixedToRoundedInt(centerVertex.p.y); scanline < scanlineMax; scanline++ ) {
		int32_t xMax = FixedToRoundedInt(rightX);
		for(int32_t x = FixedToRoundedInt(leftX); x <= xMax; x++) {
			image[x+scanline*WIDTH] = (FixedToRoundedInt(colR)<<5) | (FixedToRoundedInt(colG)<<2) | (FixedToRoundedInt(colB));
			colR += colRdX;
			colG += colGdX;
			colB += colBdX;
		}
		leftX += leftXd;
		rightX += rightXd;
		leftColR += leftColRd;
		colR = leftColR;
		leftColG += leftColGd;
		colG = leftColG;
		leftColB += leftColBd;
		colB = leftColB;
	}
}

ss_vertex_t transformedVertices[numVertices+numVertices_rad];
index_triangle_t sortedTriangles[numFaces+numFaces_rad];

static int triAvgDepthCompare(const void *p1, const void *p2) {
	index_triangle_t* t1 = (index_triangle_t*)p1;
	index_triangle_t* t2 = (index_triangle_t*)p2;
	return(
		transformedVertices[t2->v[0]].p.z +
		transformedVertices[t2->v[1]].p.z +
		transformedVertices[t2->v[2]].p.z -
		transformedVertices[t1->v[0]].p.z -
		transformedVertices[t1->v[1]].p.z -
		transformedVertices[t1->v[2]].p.z
	);
}

void RasterizeTest(uint8_t* image) {
	static int32_t rotcnt;
	
	// Projection matrix
	imat4x4_t proj = imat4x4diagonalperspective(IntToFixed(45),idiv(IntToFixed(WIDTH),IntToFixed(HEIGHT)),4096,IntToFixed(60));
	
	// Modelview matrix
	imat4x4_t modelview = imat4x4affinemul(imat4x4translate(ivec3(IntToFixed(0),IntToFixed(0),IntToFixed(-30))),imat4x4rotatex(rotcnt*24));
	modelview = imat4x4affinemul(modelview,imat4x4rotatez(rotcnt * 12));

	imat4x4_t modelview_rad = imat4x4affinemul(imat4x4translate(ivec3(IntToFixed(0),IntToFixed(0),IntToFixed(-30))),imat4x4rotatey(rotcnt*16));
	modelview_rad = imat4x4affinemul(modelview_rad,imat4x4rotatez(rotcnt * 22));
	
	// Transform
	vertex_t transformVertex;
	for(int32_t i = 0; i < numVertices; i++) {
		transformVertex.p = imat4x4transform(modelview,vertices[i].p);

		// Project
		transformVertex.p = imat4x4transform(proj,transformVertex.p);

		// Perspective divide and viewport transform
		transformedVertices[i].p = ivec3(
			Viewport(transformVertex.p.x,transformVertex.p.w,WIDTH),
			Viewport(transformVertex.p.y,transformVertex.p.w,HEIGHT),
			transformVertex.p.z
		);
		transformedVertices[i].c = vertices[i].c;
	}
	for(int32_t i = 0; i < numVertices_rad; i++) {
		transformVertex.p = imat4x4transform(modelview_rad,vertices_rad[i].p);

		// Project
		transformVertex.p = imat4x4transform(proj,transformVertex.p);

		// Perspective divide and viewport transform
		transformedVertices[i+numVertices].p = ivec3(
			Viewport(transformVertex.p.x,transformVertex.p.w,WIDTH),
			Viewport(transformVertex.p.y,transformVertex.p.w,HEIGHT),
			transformVertex.p.z
		);
		transformedVertices[i+numVertices].c = vertices_rad[i].c;
	}


	// Depth sort
	memcpy(sortedTriangles,faces,sizeof(index_triangle_t)*numFaces);
	memcpy(sortedTriangles+numFaces,faces_rad,sizeof(index_triangle_t)*numFaces_rad);
	qsort(sortedTriangles,numFaces+numFaces_rad,sizeof(index_triangle_t),&triAvgDepthCompare);
	
	// For each triangle
	triangle_t tri;
	for(int32_t i = 0; i < numFaces + numFaces_rad; i++ ) {
		tri.v[0] = transformedVertices[sortedTriangles[i].v[0]];
		tri.v[1] = transformedVertices[sortedTriangles[i].v[1]];
		tri.v[2] = transformedVertices[sortedTriangles[i].v[2]];
		RasterizeTriangle(image, tri, modelview, proj);
	}
	
	rotcnt++;
}
