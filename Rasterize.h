#ifndef __RASTERIZE_H__
#define __RASTERIZE_H__

#include "VectorLibrary/VectorFixed.h"
#include "VectorLibrary/MatrixFixed.h"

void RasterizeInit();
void Rasterize();

#define numVertices 360
#define numFaces 716
#define numVertices_rad 192
#define numFaces_rad 384

typedef struct {
	ivec4_t p;
	ivec3_t n;
	uint32_t c;
} vertex_t;

typedef struct {
	ivec3_t p;
	ivec3_t n;
} init_vertex_t;

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

#define NumberOfDotStars 300

struct RasterizeData {
	ss_vertex_t transformedVertices[numVertices+numVertices_rad];
	index_triangle_t sortedTriangles[numFaces+numFaces_rad];
	struct DotStar1 {
		int x,y,dx,f;
	} dotstars[NumberOfDotStars];
};

#endif
