#include "Radial.h"
#include "Global.h"
#include "VGA.h"
#include "LED.h"
#include "Utils.h"
#include "Button.h"

#include "Graphics/Bitmap.h"
#include "Graphics/Drawing.h"
#include "Graphics/Font.h"

extern Font OL16Font;

#define SAMPLES 4
#define WIDTH 106
#define HEIGHT 66

#include "RadialTables.h"

#include <stdint.h>
#include <string.h>

static void RenderRadial(int t,uint8_t *screen,
const uint32_t *lookup1,const uint32_t *lookup2,const uint32_t *lookup3,const uint32_t *lookup4);
static void Recolour(uint8_t *gray,uint8_t *screen);

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
	while(!UserButtonState())
	{
		WaitVBL();

		Bitmap *currframe;
		if(frame&1) { currframe=&frame2; SetFrameBuffer(framebuffer1); }
		else { currframe=&frame1; SetFrameBuffer(framebuffer2); }

		int t=VGAFrameCounter()-first;

		ClearBitmap(&ccmframe);

		int x=WIDTH*2-t;
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


		x=WIDTH*2-t;
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

	while(UserButtonState());
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
	#if 0

	register uint32_t r4 asm("r4"),r5 asm("r5"),r6 asm("r6");
	register uint32_t r7 asm("r7"),r8 asm("r8"),r9 asm("r9");

	// r0: sum
	// r1: val/yblend
	// r2: tmp
	// r3: xblend
	// r4: lookup
	// r5: pixel
	// r6: num_x
	// r7: num_y
	// r8: lookup_skip
	// r9: pixel_skip
	// r10: x_counter
	// r11: offspixel
	// r12: loadtmp
	// r14:

	#define ONE_LOOKUP \
	"	ldr		r1,[r4],#4			\n" \
	"	add		r11,r5,r1,asr #16	\n" \
	"	and		r3,r1,#0x00ff		\n" \
	"	and		r1,r1,#0xff00		\n" \
	"	mov		r1,r1,lsr #8		\n" \
	                                    \
	"	ldrb	r12,[r11,#129]		\n" \
	"	smulbb	r2,r12,r1			\n" \
	"	rsb		r1,r1,#128			\n" \
	"	ldrb	r12,[r11,#128]		\n" \
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
	r5=(uint32_t)&screen[cx-1+(cy-1)*128];
	r6=cx;
	r7=cy;
	r8=(WIDTH-cx)*SAMPLES*4;
	r9=-(WIDTH-cx);

	asm volatile(
	"yloop1:						\n"
	"	mov		r10,r6				\n"

	"xloop1:						\n"
	"	ldrb	r0,[r5]				\n"
	"	mov		r0,r0,lsl #16		\n"

	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP

	"	mov		r0,r0,lsr #16		\n"
	"	cmp		r0,#255				\n"
	"	movgt	r0,#255				\n"
	"	strb	r0,[r5],#-1			\n"

	"	subs	r10,r10,#1			\n"
	"	bne		xloop1				\n"

	"	add		r4,r4,r8			\n"
	"	add		r5,r5,r9			\n"
	"	subs	r7,r7,#1			\n"
	"	bne		yloop1				\n"

	:"=r" (r4),"=r" (r5),"=r" (r7)
	:"r" (r4),"r" (r5),"r" (r6),"r" (r7),"r" (r8),"r" (r9)
	:"r0","r1","r2","r3","r10","r11","r12","cc"
	);

	r4=(uint32_t)lookup2;
	r5=(uint32_t)&screen[cx+(cy-1)*128];
	r6=WIDTH-cx;
	r7=cy;
	r8=cx*SAMPLES*4;
	r9=cx-2*WIDTH;

	asm volatile(
	"yloop2:						\n"
	"	mov		r10,r6				\n"

	"xloop2:						\n"
	"	ldrb	r0,[r5]				\n"
	"	mov		r0,r0,lsl #16		\n"

	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP

	"	mov		r0,r0,lsr #16		\n"
	"	cmp		r0,#255				\n"
	"	movgt	r0,#255				\n"
	"	strb	r0,[r5],#1			\n"

	"	subs	r10,r10,#1			\n"
	"	bne		xloop2				\n"

	"	add		r4,r4,r8			\n"
	"	add		r5,r5,r9			\n"
	"	subs	r7,r7,#1			\n"
	"	bne		yloop2				\n"

	:"=r" (r4),"=r" (r5),"=r" (r7)
	:"r" (r4),"r" (r5),"r" (r6),"r" (r7),"r" (r8),"r" (r9)
	:"r0","r1","r2","r3","r10","r11","r12","cc"
	);

	r4=(uint32_t)lookup3;
	r5=(uint32_t)&screen[(cx-1)+cy*128];
	r6=cx;
	r7=HEIGHT-cy;
	r8=(WIDTH-cx)*SAMPLES*4;
	r9=cx+WIDTH;

	asm volatile(
	"yloop3:						\n"
	"	mov		r10,r6				\n"

	"xloop3:						\n"
	"	ldrb	r0,[r5]				\n"
	"	mov		r0,r0,lsl #16		\n"

	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP

	"	mov		r0,r0,lsr #16		\n"
	"	cmp		r0,#255				\n"
	"	movgt	r0,#255				\n"
	"	strb	r0,[r5],#-1			\n"

	"	subs	r10,r10,#1			\n"
	"	bne		xloop3				\n"

	"	add		r4,r4,r8			\n"
	"	add		r5,r5,r9			\n"
	"	subs	r7,r7,#1			\n"
	"	bne		yloop3				\n"

	:"=r" (r4),"=r" (r5),"=r" (r7)
	:"r" (r4),"r" (r5),"r" (r6),"r" (r7),"r" (r8),"r" (r9)
	:"r0","r1","r2","r3","r10","r11","r12","cc"
	);

	r4=(uint32_t)lookup4;
	r5=(uint32_t)&screen[cx+cy*128];
	r6=WIDTH-cx;
	r7=HEIGHT-cy;
	r8=cx*SAMPLES*4;
	r9=cx;

	asm volatile(
	"yloop4:						\n"
	"	mov		r10,r6				\n"

	"xloop4:						\n"
	"	ldrb	r0,[r5]				\n"
	"	mov		r0,r0,lsl #16		\n"

	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP
	ONE_LOOKUP

	"	mov		r0,r0,lsr #16		\n"
	"	cmp		r0,#255				\n"
	"	movgt	r0,#255				\n"
	"	strb	r0,[r5],#1			\n"

	"	subs	r10,r10,#1			\n"
	"	bne		xloop4				\n"

	"	add		r4,r4,r8			\n"
	"	add		r5,r5,r9			\n"
	"	subs	r7,r7,#1			\n"
	"	bne		yloop4				\n"

	:"=r" (r4),"=r" (r5),"=r" (r7)
	:"r" (r4),"r" (r5),"r" (r6),"r" (r7),"r" (r8),"r" (r9)
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
