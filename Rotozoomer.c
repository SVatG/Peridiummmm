#include "Rotozoomer.h"
#include "VGA.h"
#include "LED.h"
#include "Button.h"
#include "Utils.h"

#include "Graphics/Pixels.h"

#include <stdint.h>
#include <string.h>

static void RotozoomHSYNCHandler();
static uint32_t sqrti(uint32_t n);

static volatile int32_t x0,y0,dx,dy;

static uint32_t PackCoordinates(int32_t x,int32_t y)
{
	x&=0x3ffff;
	y&=0x3ff80;
	return (y>>(12-16+5))|(x<<20)|(x>>12);
}

static volatile uint32_t Pos,Delta;
static uint8_t linetexture[480];

void Rotozoom()
{
	memset(linetexture,0,sizeof(linetexture));

	uint8_t *texture=(uint8_t *)0x20000000;

	for(int i=0;i<32;i++)
	{
		for(int y=0;y<32;y++)
		for(int x=0;x<32;x++)
		{
			int dx=2*(2*x+1)-64;
			int dy=2*(2*y+1)-64;
			int r=sqrti(dx*dx+dy*dy);

			int red=0,green=0,blue=0;

			if(r<2*i)
			{
				red=(2*i-r)/3;
				if(red>7)
				{
					green=red-8;
					red=7;
					if(green>7)
					{
						blue=green-8;
						green=7;

						if(blue>3) blue=3;
					}
				}
			}

			int offset=(y<<(16-5))|(i<<5)|x;
			texture[offset]=RawRGB(red,green,blue);
		}
	}

	SetVGAHorizontalSync31kHz(RotozoomHSYNCHandler);

	SetLEDs(0x5);

	while(!UserButtonState())
	{
		WaitVBL();
		int t=VGAFrameCounter();

		SetLEDs(1<<((t/3)&3));

		int32_t angle=isin(t*9)&1023;
		int32_t scale=(icos(t*17)+Fix(2))/2;

		dx=imul(scale,icos(angle));
		dy=imul(scale,isin(angle));

		x0=-dx*320-dy*200;
		y0=-(dy&0xffffff80)*320+dx*200;
		Pos=PackCoordinates(x0,y0);
		Delta=PackCoordinates(dx,dy);

		for(int y=0;y<480;y++)
		{
			int pos=icos(t*20);
			linetexture[y]=(32*(isin(y*6+pos)+Fix(1))/8192);
			if(linetexture[y]>31) linetexture[y]=31;
		}
	}

	while(UserButtonState());
}

static void RotozoomHSYNCHandler()
{
	int line=HandleVGAHSync200();
	if(line<0) return;

	register uint32_t r0 __asm__("r0")=Pos;
	register uint32_t r1 __asm__("r1")=Delta;
	register uint32_t r2 __asm__("r2")=0xf81f;
	register uint32_t r3 __asm__("r3")=0x20000000+(linetexture[line]<<5);
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

	x0+=dy;
	y0-=dx;
	Pos=PackCoordinates(x0,y0);
}

static uint32_t sqrti(uint32_t n)
{
	uint32_t s,t;

	#define sqrtBit(k) \
	t = s+(1UL<<(k-1)); t <<= k+1; if (n >= t) { n -= t; s |= 1UL<<k; }

	s=0;
	if(n>=1<<30) { n-=1<<30; s=1<<15; }
	sqrtBit(14); sqrtBit(13); sqrtBit(12); sqrtBit(11); sqrtBit(10);
	sqrtBit(9); sqrtBit(8); sqrtBit(7); sqrtBit(6); sqrtBit(5);
	sqrtBit(4); sqrtBit(3); sqrtBit(2); sqrtBit(1);
	if(n>s<<1) s|=1;

	#undef sqrtBit

	return s;
}
