#include "Radial.h"
#include "Global.h"
#include "VGA.h"
#include "LED.h"
#include "Utils.h"
#include "Button.h"

#include "Graphics/Bitmap.h"
#include "Graphics/Drawing.h"
#include "Graphics/Font.h"
#include "Graphics/GenericDrawing.h"

extern Font OL16Font;

#define SAMPLES 4
#define WIDTH 106
#define HEIGHT 66

#include "RadialTables.h"

#include <stdint.h>
#include <string.h>

#include "Rasterize.h"

#define Viewport(x,w,s) (imul(idiv((x),(w))+IntToFixed(1),IntToFixed((s)/2)))

static void RenderRadial(int t,uint8_t *screen,
const uint32_t *lookup1,const uint32_t *lookup2,const uint32_t *lookup3,const uint32_t *lookup4);
static void Recolour(uint8_t *gray,uint8_t *screen);

#define cubeLine(i,j) DrawLineDash( \
		currframe,  \
		FixedToInt(transformedVertices[i].p.x),  \
		FixedToInt(transformedVertices[i].p.y), \
		FixedToInt(transformedVertices[j].p.x), \
		FixedToInt(transformedVertices[j].p.y), \
		RawRGB(7,7,3) \
	)

#define visible(i,j,k) (imul(transformedVertices[j].p.x - transformedVertices[i].p.x, transformedVertices[k].p.y - transformedVertices[i].p.y) - \
		        imul(transformedVertices[k].p.x - transformedVertices[i].p.x, transformedVertices[j].p.y - transformedVertices[i].p.y) \
		        > 0)

#define cubeSide(a,b,c,d) \
	cubeLine(a,b); \
	cubeLine(b,c); \
	cubeLine(c,d); \
	cubeLine(d,a);
	
static inline void GenericDrawLineDash(Bitmap *bitmap,int x1,int y1,int x2,int y2,Pixel c,CompositionMode comp,
GenericDrawPixelFunction *pixelfunc,GenericDrawHorizontalLineFunction *hlinefunc,GenericDrawVerticalLineFunction *vlinefunc)
{
	int pixelcnt = 0;
	
	if(x1==x2)
	{
		if(y1<y2) GenericDrawVerticalLine(bitmap,x1,y1,y2-y1+1,c,comp,vlinefunc);
		else GenericDrawVerticalLine(bitmap,x1,y2,y1-y2+1,c,comp,vlinefunc);
		return;
	}
	else if(y1==y2)
	{
		if(x1<x2) GenericDrawHorizontalLine(bitmap,x1,y1,x2-x1+1,c,comp,hlinefunc);
		else GenericDrawHorizontalLine(bitmap,x1,y2,x1-x2+1,c,comp,hlinefunc);
		return;
	}

	bool steep=abs(y2-y1)>abs(x2-x1);

	if(steep)
	{
		int t;
		t=x1; x1=y1; y1=t;
		t=x2; x2=y2; y2=t;
	}

	if(x1>x2)
	{
		int t;
		t=x1; x1=x2; x2=t;
		t=y1; y1=y2; y2=t;
	}

	int deltax=x2-x1;
	int deltay,ydir;
	if(y1<y2) { deltay=y2-y1; ydir=1; }
	else { deltay=y1-y2; ydir=-1; }

	int error=deltax>>1;
	int y=y1;

	if(steep)
	{
		for(int x=x1;x<=x2;x++)
		{
			pixelcnt = (pixelcnt + 1) % 8;
			if(pixelcnt < 4) {
				GenericDrawPixel(bitmap,y,x,c,comp,pixelfunc);
			}
			error-=deltay;
			if(error<0)
			{
				y+=ydir;
				error+=deltax;
			}
		}
	}
	else
	{
		for(int x=x1;x<=x2;x++)
		{
			pixelcnt = (pixelcnt + 1) % 8;
			if(pixelcnt < 4) {
				GenericDrawPixel(bitmap,x,y,c,comp,pixelfunc);
			}
			error-=deltay;
			if(error<0)
			{
				y+=ydir;
				error+=deltax;
			}
		}
	}
}

static inline void DrawLineDash(Bitmap *bitmap,int x1,int y1,int x2,int y2,Pixel c) {
	GenericDrawLineDash(bitmap,x1,y1,x2,y2,c,NULL,DrawPixelNoClipFunction,DrawHorizontalLineNoClipFunction,DrawVerticalLineNoClipFunction);
}

void CubeBG(Bitmap* currframe, int32_t rotcnt) {

	#include "cube.h"
	
	// Projection matrix
	imat4x4_t proj = imat4x4diagonalperspective(IntToFixed(45),idiv(IntToFixed(WIDTH*2),IntToFixed(HEIGHT*2)),128,IntToFixed(14));

	// Modelview matrix
	imat4x4_t modelview = imat4x4affinemul(imat4x4translate(ivec3(IntToFixed(0),IntToFixed(0),IntToFixed(-5))),imat4x4rotatex(rotcnt*8));
	modelview = imat4x4affinemul(modelview,imat4x4rotatez(rotcnt * 4));

	vertex_t transformVertex;
	ss_vertex_t transformedVertices[8];
	for(int32_t i = 0; i < numCubeVertices; i++) {
		transformVertex.p = imat4x4transform(modelview,ivec4(cubeVertices[i].p.x,cubeVertices[i].p.y,cubeVertices[i].p.z,F(1)));

		// Project
		transformVertex.p = imat4x4transform(proj,transformVertex.p);

		// Perspective divide and viewport transform
		transformedVertices[i].p = ivec3(
			Viewport(transformVertex.p.x,transformVertex.p.w,WIDTH*2),
			Viewport(transformVertex.p.y,transformVertex.p.w,HEIGHT*2),
			transformVertex.p.z
		);
	}

	if(visible(0,1,2)) {
		cubeSide(0,1,2,3);
	}

	if(visible(4,6,5)) {
		cubeSide(4,5,6,7);
	}

	if(visible(1,6,2)) {
		cubeSide(1,5,6,2);
	}

	if(visible(4,3,7)) {
		cubeSide(4,0,3,7);
	}

	if(visible(4,1,0)) {
		cubeSide(4,5,1,0);
	}

	if(visible(7,3,2)) {
		cubeSide(7,6,2,3);
	}
}

void RadialScroller(const char *text)
{
	uint8_t *framebuffer1=(uint8_t *)0x20000000;
	uint8_t *framebuffer2=(uint8_t *)0x20010000;
	memset(framebuffer1,0,320*200);
	memset(framebuffer2,0,320*200);

	SetVGAScreenMode212x133(framebuffer1);

	Bitmap frame1,frame2,ccmframe;
	InitializeBitmap(&frame1,212,133,212,framebuffer1);
	InitializeBitmap(&frame2,212,133,212,framebuffer2);
	InitializeBitmap(&ccmframe,WIDTH*2,HEIGHT*2,WIDTH*2,data.radial.frame);

	int frame=0;
	int nearestindex=0;
	int first=VGAFrameCounter();

int last_t=0;
	while(CurrentBitBinRow(&song) < 1728)
	{
		WaitVBL();

		Bitmap *currframe;
		if(frame&1) { currframe=&frame2; SetFrameBuffer(framebuffer1); }
		else { currframe=&frame1; SetFrameBuffer(framebuffer2); }

		int t=VGAFrameCounter()-first;

		ClearBitmap(&ccmframe);

		CubeBG(&ccmframe,t);
		
		int x=WIDTH*2-t*3;
		for(const char *ptr=text;*ptr;ptr++)
		{
			int c=*ptr;

			int y=(isin(x*20+t*33)>>8)+HEIGHT-8;

			DrawCharacter(&ccmframe,&OL16Font,x,y,255,c);

			x+=WidthOfCharacter(&OL16Font,c)+2;
		}

//		DrawString(&ccmframe,&OL16Font,320-t,HEIGHT+10,255,text);
//		DrawString(&ccmframe,&OL16Font,320-t,HEIGHT-10-16,255,text);

		RenderRadial(t,ccmframe.pixels,lookup1,lookup2,lookup3,lookup4);

//		DrawString(&ccmframe,&OL16Font,320-t,HEIGHT+10,255,text);
//		DrawString(&ccmframe,&OL16Font,320-t,HEIGHT-10-16,255,text);

		Recolour(ccmframe.pixels,currframe->pixels);

		x=WIDTH*2-t*3;
		for(const char *ptr=text;*ptr;ptr++)
		{
			int c=*ptr;

			int y=(isin(x*20+t*33)>>8)+HEIGHT-8;

			DrawCharacter(currframe,&OL16Font,x,y,255,c);

			x+=WidthOfCharacter(&OL16Font,c)+2;
		}

last_t=t;

		frame++;
	}

// 	while(UserButtonState());
}

static inline void RenderPixel(uint8_t *pixel,const uint32_t *lookup)
{
	int sum=*pixel<<0+14;
	for(int i=0;i<SAMPLES;i++)
	{
		int val=*lookup++;
		int16_t offs=val>>16;
		int xblend=(val>>8)&0xff;
		int yblend=(val>>0)&0xff;

		sum+=(pixel[offs]*(128-xblend)+pixel[offs+1]*xblend)*(128-yblend)
		+(pixel[offs+WIDTH*2]*(128-xblend)+pixel[offs+WIDTH*2+1]*xblend)*yblend;
	}

	sum>>=2+14;
	if(sum>255) sum=255;
	*pixel=sum;
}

static void RenderRadial(int t,uint8_t *screen,
const uint32_t *lookup1,const uint32_t *lookup2,const uint32_t *lookup3,const uint32_t *lookup4)
{
	#if 1

	register uint32_t r4 __asm__("r4"),r5 __asm__("r5"),r6 __asm__("r6");
	register uint32_t r7 __asm__("r7");

	// r0: sum
	// r1: val/yblend
	// r2: tmp
	// r3: xblend
	// r4: lookup
	// r5: pixel
	// r6: num_x
	// r7: num_y
	// r10: x_counter
	// r11: offspixel
	// r12: loadtmp
	// r14:

int cx=WIDTH;
int cy=HEIGHT;

	#define ONE_LOOKUP \
	"	ldr		r1,[r4],#4			\n" \
	"	add		r11,r5,r1,asr #16	\n" \
	"	and		r3,r1,#0x00ff		\n" \
	"	and		r1,r1,#0xff00		\n" \
	"	mov		r1,r1,lsr #8		\n" \
	                                    \
	"	ldrb	r12,[r11,#213]		\n" \
	"	smulbb	r2,r12,r1			\n" \
	"	rsb		r1,r1,#128			\n" \
	"	ldrb	r12,[r11,#212]		\n" \
	"	smlabb	r2,r12,r1,r2		\n" \
	"	smlabb	r0,r2,r3,r0			\n" \
	"	rsb		r3,r3,#128			\n" \
	                                    \
	"	ldrb	r12,[r11,#0]		\n" \
	"	smulbb	r2,r12,r1			\n" \
	"	rsb		r1,r1,#128			\n" \
	"	ldrb	r12,[r11,#1]		\n" \
	"	smlabb	r2,r12,r1,r2		\n" \
	"	smlabb	r0,r2,r3,r0			\n"

	r4=(uint32_t)lookup1;
	r5=(uint32_t)&screen[WIDTH-1+(HEIGHT-1)*WIDTH*2];
	r6=WIDTH;
	r7=HEIGHT;

	__asm__ volatile(
	"yloop1:						\n"
	"	mov		r10,r6				\n"

	"xloop1:						\n"
	"	ldrb	r0,[r5]				\n"
	"	mov		r0,r0,lsl #14		\n"

	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP

	"	mov		r0,r0,lsr #16		\n"
	"	cmp		r0,#255				\n"
	"	it		gt					\n"
	"	movgt	r0,#255				\n"
	"	strb	r0,[r5],#-1			\n"

	"	subs	r10,r10,#1			\n"
	"	bne		xloop1				\n"

	"	sub		r5,r5,#106			\n"
	"	subs	r7,r7,#1			\n"
	"	bne		yloop1				\n"

	:"=r" (r4),"=r" (r5),"=r" (r7)
	:"r" (r4),"r" (r5),"r" (r6),"r" (r7)
	:"r0","r1","r2","r3","r10","r11","r12","cc"
	);

	r4=(uint32_t)lookup2;
	r5=(uint32_t)&screen[WIDTH+(HEIGHT-1)*WIDTH*2];
	r6=WIDTH;
	r7=HEIGHT;

	__asm__ volatile(
	"yloop2:						\n"
	"	mov		r10,r6				\n"

	"xloop2:						\n"
	"	ldrb	r0,[r5]				\n"
	"	mov		r0,r0,lsl #14		\n"

	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP

	"	mov		r0,r0,lsr #16		\n"
	"	cmp		r0,#255				\n"
	"	it		gt					\n"
	"	movgt	r0,#255				\n"
	"	strb	r0,[r5],#1			\n"

	"	subs	r10,r10,#1			\n"
	"	bne		xloop2				\n"

	"	sub		r5,r5,#318			\n"
	"	subs	r7,r7,#1			\n"
	"	bne		yloop2				\n"

	:"=r" (r4),"=r" (r5),"=r" (r7)
	:"r" (r4),"r" (r5),"r" (r6),"r" (r7)
	:"r0","r1","r2","r3","r10","r11","r12","cc"
	);

	r4=(uint32_t)lookup3;
	r5=(uint32_t)&screen[(WIDTH-1)+HEIGHT*WIDTH*2];
	r6=WIDTH;
	r7=HEIGHT;

	__asm__ volatile(
	"yloop3:						\n"
	"	mov		r10,r6				\n"

	"xloop3:						\n"
	"	ldrb	r0,[r5]				\n"
	"	mov		r0,r0,lsl #14		\n"

	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP

	"	mov		r0,r0,lsr #16		\n"
	"	cmp		r0,#255				\n"
	"	it		gt					\n"
	"	movgt	r0,#255				\n"
	"	strb	r0,[r5],#-1			\n"

	"	subs	r10,r10,#1			\n"
	"	bne		xloop3				\n"

	"	add		r5,r5,#318			\n"
	"	subs	r7,r7,#1			\n"
	"	bne		yloop3				\n"

	:"=r" (r4),"=r" (r5),"=r" (r7)
	:"r" (r4),"r" (r5),"r" (r6),"r" (r7)
	:"r0","r1","r2","r3","r10","r11","r12","cc"
	);

	r4=(uint32_t)lookup4;
	r5=(uint32_t)&screen[WIDTH+HEIGHT*WIDTH*2];
	r6=WIDTH;
	r7=HEIGHT;

	__asm__ volatile(
	"yloop4:						\n"
	"	mov		r10,r6				\n"

	"xloop4:						\n"
	"	ldrb	r0,[r5]				\n"
	"	mov		r0,r0,lsl #14		\n"

	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP

	"	mov		r0,r0,lsr #16		\n"
	"	cmp		r0,#255				\n"
	"	it		gt					\n"
	"	movgt	r0,#255				\n"
	"	strb	r0,[r5],#1			\n"

	"	subs	r10,r10,#1			\n"
	"	bne		xloop4				\n"

	"	add		r5,r5,#106			\n"
	"	subs	r7,r7,#1			\n"
	"	bne		yloop4				\n"

	:"=r" (r4),"=r" (r5),"=r" (r7)
	:"r" (r4),"r" (r5),"r" (r6),"r" (r7)
	:"r0","r1","r2","r3","r10","r11","r12","cc"
	);

	#else

	const uint32_t *lookup;
	uint8_t *pixel;

	lookup=lookup1;
	pixel=&screen[WIDTH-1+(HEIGHT-1)*WIDTH*2];
	for(int y=HEIGHT-1;y>=0;y--)
	{
		for(int x=WIDTH-1;x>=0;x--)
		{
			RenderPixel(pixel,lookup);
			lookup+=SAMPLES;
			pixel--;
		}
		pixel-=WIDTH;
	}

	lookup=lookup2;
	pixel=&screen[WIDTH+(HEIGHT-1)*WIDTH*2];
	for(int y=HEIGHT-1;y>=0;y--)
	{
		for(int x=WIDTH;x<2*WIDTH;x++)
		{
			RenderPixel(pixel,lookup);
			lookup+=SAMPLES;
			pixel++;
		}
		pixel-=3*WIDTH;
	}

	lookup=lookup3;
	pixel=&screen[(WIDTH-1)+HEIGHT*WIDTH*2];
	for(int y=HEIGHT;y<2*HEIGHT;y++)
	{
		for(int x=WIDTH-1;x>=0;x--)
		{
			RenderPixel(pixel,lookup);
			lookup+=SAMPLES;
			pixel--;
		}
		pixel+=3*WIDTH;
	}

	lookup=lookup4;
	pixel=&screen[WIDTH+HEIGHT*WIDTH*2];
	for(int y=HEIGHT;y<2*HEIGHT;y++)
	{
		for(int x=WIDTH;x<2*WIDTH;x++)
		{
			RenderPixel(pixel,lookup);
			lookup+=SAMPLES;
			pixel++;
		}
		pixel+=WIDTH;
	}

	#endif
}

static void Recolour(uint8_t *gray,uint8_t *screen)
{
	uint32_t *src=(uint32_t *)gray;
	uint32_t *dest=(uint32_t *)screen;

	for(int i=0;i<2*WIDTH*2*HEIGHT/4;i++)
	{
		uint32_t val=*src++;
		val&=0xf8f8f8f8;
		val>>=3;
		val|=(val>>3)&0x03030303;
		*dest++=val;
	}
}
