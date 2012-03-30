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
	int first=VGAFrameCounter();

	Bitmap textbitmap;
	InitializeBitmap(&textbitmap,4000,16,4000,(uint8_t *)0x20010000);
	ClearBitmap(&textbitmap);
	DrawString(&textbitmap,&OLFont,320,0,0,text);

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

	for(int i=0;i<8;i++)
	{
		data.copper[0][i]=RGB(i*32,0,0);
		data.copper[0][15-i]=RGB(i*32,0,0);
		data.copper[1][i]=RGB(i*32,i*32,0);
		data.copper[1][15-i]=RGB(i*32,i*32,0);
		data.copper[2][i]=RGB(0,i*32,0);
		data.copper[2][15-i]=RGB(0,i*32,0);
		data.copper[3][i]=RGB(0,i*32,i*32);
		data.copper[3][15-i]=RGB(0,i*32,i*32);
		data.copper[4][i]=RGB(0,0,i*32);
		data.copper[4][15-i]=RGB(0,0,i*32);
		data.copper[5][i]=RGB(i*32,0,i*32);
		data.copper[5][15-i]=RGB(i*32,0,i*32);
	}

	data.scanline=0x20010000;

	SetVGAHorizontalSync31kHz(ScrollerHSyncHandler);

	while(!UserButtonState())
	{
		WaitVBL();
		int t=VGAFrameCounter()-first;

		int offs=t*2;
		if(offs>4000-320) offs=4000-320;
		data.scanline=0x20010000+offs;

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

		int copperangle=t*32;
		int copperphase=copperangle%(4096/6);
		int coppercol=copperangle/(4096/6);

		int coppery0=FixedToInt(isin(copperphase-512)*16)+16;
		int coppery1=FixedToInt(isin(copperphase-512+4096*1/6)*16)+16;
		int coppery2=FixedToInt(isin(copperphase-512+4096*2/6)*16)+16;
		int coppery3=FixedToInt(isin(copperphase-512+4096*3/6)*16)+16;
		int coppery4=FixedToInt(isin(copperphase-512+4096*4/6)*16)+16;
		int coppery5=FixedToInt(isin(copperphase-512+4096*5/6)*16)+16;
		int coppercol0=((6-coppercol)%6+6)%6;
		int coppercol1=(coppercol0+1)%6;
		int coppercol2=(coppercol0+2)%6;
		int coppercol3=(coppercol0+3)%6;
		int coppercol4=(coppercol0+4)%6;
		int coppercol5=(coppercol0+5)%6;

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
			else if(y<128+48)
			{
				int ty=y-128;

				if(ty>=coppery0 && ty<coppery0+16) data.lines[y].texture=data.copper[coppercol0][ty-coppery0];
				else if(ty>=coppery1 && ty<coppery1+16) data.lines[y].texture=data.copper[coppercol1][ty-coppery1];
				else if(ty>=coppery5 && ty<coppery5+16) data.lines[y].texture=data.copper[coppercol5][ty-coppery5];
				else if(ty>=coppery2 && ty<coppery2+16) data.lines[y].texture=data.copper[coppercol2][ty-coppery2];
				else if(ty>=coppery4 && ty<coppery4+16) data.lines[y].texture=data.copper[coppercol4][ty-coppery4];
				else if(ty>=coppery3 && ty<coppery3+16) data.lines[y].texture=data.copper[coppercol3][ty-coppery3];
				else data.lines[y].texture=0;
			}
			else if(y>=400-128-48)
			{
				int ty=47-(y-(400-128-48));

				if(ty>=coppery0 && ty<coppery0+16) data.lines[y].texture=data.copper[coppercol0][ty-coppery0];
				else if(ty>=coppery1 && ty<coppery1+16) data.lines[y].texture=data.copper[coppercol1][ty-coppery1];
				else if(ty>=coppery5 && ty<coppery5+16) data.lines[y].texture=data.copper[coppercol5][ty-coppery5];
				else if(ty>=coppery2 && ty<coppery2+16) data.lines[y].texture=data.copper[coppercol2][ty-coppery2];
				else if(ty>=coppery4 && ty<coppery4+16) data.lines[y].texture=data.copper[coppercol4][ty-coppery4];
				else if(ty>=coppery3 && ty<coppery3+16) data.lines[y].texture=data.copper[coppercol3][ty-coppery3];
				else data.lines[y].texture=0;
			}
		}
	}

	SetBlankVGAScreenMode200();

	while(UserButtonState());
}

static void ScrollerHSyncHandler()
{
	int line=HandleVGAHSync200();
	if(line<0) return;

	if(line<128||line>=400-128)
	{
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
	
		SetVGASignalToBlack();
	}
	else if(line>=200-16 && line<200+16)
	{
		register uint32_t r0 __asm__("r0")=data.scanline;
		register uint32_t r1 __asm__("r1")=((uint32_t)&GPIOE->ODR)+1;
		register uint32_t r2 __asm__("r2")=320;

		__asm__ volatile(
		"0:						\n"
		"	ldrb	r3,[r0]		\n"
		"	strb	r3,[r1]		\n"
		"	add		r0,r0,#1	\n"
		"	nop					\n"
		"	nop					\n"
		"	nop					\n"
		"	nop					\n"
		"	nop					\n"
		"	nop					\n"
		"	subs	r2,r2,#1	\n"
		"	bne		0b			\n"
		:
		:"r" (r0), "r" (r1), "r" (r2)
		:"r3");

		SetVGASignalToBlack();
		if(line&1) data.scanline+=4000;
	}
	else
	{
		SetVGASignal(data.lines[line].texture);

		register uint32_t r0 __asm__("r0")=1400;

		__asm__ volatile(
		"0:						\n"
		"	subs	r0,r0,#1	\n"
		"	bne		0b			\n"
		:
		: "r" (r0)
		:);

		SetVGASignalToBlack();
	}
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
