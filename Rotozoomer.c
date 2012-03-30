#include "Rotozoomer.h"
#include "Global.h"
#include "VGA.h"
#include "LED.h"
#include "Button.h"
#include "Utils.h"

#include "Graphics/Pixels.h"

#include <stdint.h>
#include <string.h>

static void RotozoomHSYNCHandler();
static uint32_t sqrti(uint32_t n);

static uint32_t PackCoordinates(int32_t x,int32_t y)
{
	x&=0x1ffff;
	y&=0x1ff80;
	return (y>>(12-16+5))|(x<<20)|(x>>12);
}


void Rotozoom()
{
	memset(&data,0,sizeof(struct RotozoomerData));

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

		int32_t dx=imul(scale,icos(angle));
		int32_t dy=imul(scale,isin(angle));
		data.rotozoomer.dx=dx;
		data.rotozoomer.dy=dy;

		data.rotozoomer.x0=-dx*320-dy*200;
		data.rotozoomer.y0=-(dy&0xffffff80)*320+dx*200;
		data.rotozoomer.pos=PackCoordinates(data.rotozoomer.x0,data.rotozoomer.y0);
		data.rotozoomer.delta=PackCoordinates(dx,dy);

		for(int y=0;y<400;y++)
		{
			int pos=icos(t*20);
			data.rotozoomer.texture[y]=(32*(isin(y*6+pos)+Fix(1))/8192);
			if(data.rotozoomer.texture[y]>31) data.rotozoomer.texture[y]=31;
		}
	}

	while(UserButtonState());
}

static void RotozoomHSYNCHandler()
{
	int line=HandleVGAHSync200();
	if(line<0) return;

	register uint32_t r0 __asm__("r0")=data.rotozoomer.pos;
	register uint32_t r1 __asm__("r1")=data.rotozoomer.delta;
	register uint32_t r2 __asm__("r2")=0xf81f;
	register uint32_t r3 __asm__("r3")=0x20000000+(data.rotozoomer.texture[line]<<5);
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

	data.rotozoomer.x0+=data.rotozoomer.dy;
	data.rotozoomer.y0-=data.rotozoomer.dx;
	data.rotozoomer.pos=PackCoordinates(data.rotozoomer.x0,data.rotozoomer.y0);
}
