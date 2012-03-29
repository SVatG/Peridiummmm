#include "Scroller.h"
#include "VGA.h"
#include "LED.h"
#include "Button.h"
#include "Utils.h"
#include "Random.h"
#include "Graphics/Font.h"

#include "Graphics/Pixels.h"

#include <stdint.h>
#include <string.h>

static void ScrollerHSyncHandler();

static uint32_t sqrti(uint32_t n);

extern Font OLFont;

struct ScrollerData data;

static uint32_t PackCoordinates(int32_t x,int32_t y)
{
	x&=0x7fff;
	y&=0x7fff;
	return (y<<1)|(x<<18)|(x>>14);
}


void Scroller(const char *text)
{
//DrawString(currframe,&OLFont,10,10,0,"Look out honey coz I'm using technology");

	memset(&data,0,sizeof(data));

	uint8_t *texture=(uint8_t *)0x20000000;

	for(int i=0;i<(1<<14);i++)
	{
		for(int y=0;y<2;y++)
		for(int x=0;x<2;x++)
		{
			int offset=(y<<(16-1))|(i<<1)|x;
			if(x^y) texture[offset]=i;
			else texture[offset]=0;
		}
	}

	for(int y=0;y<400;y++)
	{
		data.lines[y].z=idiv(Fix(1000),iabs(Fix(y-200)));
	}

	SetVGAHorizontalSync31kHz(ScrollerHSyncHandler);

	while(!UserButtonState())
	{
		WaitVBL();
		int t=VGAFrameCounter();

		SetLEDs(1<<((t/3)&3));

		int32_t viewangle=t*9;
		int32_t pathangle=-t*5;
		int32_t scale=(icos(t*17)+Fix(2))/2;

		int r1=255,g1=0,b1=0;
		int r2=0,g2=255,b2=0;

		int32_t wdx=icos(viewangle);
		int32_t wdy=isin(viewangle);
	
		int32_t x0=40*icos(pathangle);
		int32_t y0=40*isin(pathangle);

		if(wdx<0)
		{
			wdx=-wdx;
			wdy=-wdy;
			x0=-x0;
			y0=-y0;
		}

		for(int y=0;y<400;y++)
		{
			if(y<128||y>=400-128)
			{
				int32_t z=data.lines[y].z;

				int32_t x1=x0+imul(-wdy,z);
				int32_t y1=y0+imul(wdx,z);
	
				int32_t sdx=imul(wdx,z)/300;
				int32_t sdy=imul(wdy,z)/300;
	
				int32_t x2=x1-sdx*320;
				int32_t y2=y1-sdy*320;
	
				int r,g,b;
				if(y<200)
				{
					r=r1; g=g1; b=b1;
					r*=127-y;
					g*=127-y;
					b*=127-y;
				}
				else
				{
					r=r2; g=g2; b=b2;
					r*=y-400+128;
					g*=y-400+128;
					b*=y-400+128;
				}
	
				r>>=7;
				g>>=7;
				b>>=7;
	
/*				if((RandomInteger()&31)<(r&31)) r+=32;
				if((RandomInteger()&31)<(g&31)) g+=32;
				if((RandomInteger()&63)<(b&63)) b+=64;*/

				data.lines[y].pos=PackCoordinates(x2,y2);
				data.lines[y].delta=PackCoordinates(sdx,sdy);
				data.lines[y].texture=RGB(r,g,b);
			}
		}
	}
	while(UserButtonState());
}

static void ScrollerHSyncHandler()
{
	int line=HandleVGAHSync200();
	if(line<0) return;

	register uint32_t r0 __asm__("r0")=data.lines[line].pos;
	register uint32_t r1 __asm__("r1")=data.lines[line].delta;
	register uint32_t r2 __asm__("r2")=0x8001;
	register uint32_t r3 __asm__("r3")=0x20000000+(data.lines[line].texture<<1);
	register uint32_t r4 __asm__("r4")=0x40021015;
	#define P \
	"	adcs	r0,r1		\n" \
	"	and		r5,r0,r2	\n" \
	"	ldrb	r6,[r3,r5]	\n" \
	"	strb	r6,[r4]		\n"
				
	__asm__ volatile(
	"	b		.start		\n"
	"	.align 4	\n"
	".start:	\n"
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 

	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 

	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 

	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
				
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P  P P P P 
	#undef P
			
	".end:	\n"
	:
	: "r" (r0), "r" (r1), "r" (r2), "r" (r3), "r" (r4)
	:"r5","r6");

	((uint8_t *)&GPIOE->ODR)[1]=0;
}
