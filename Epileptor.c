#include "Epileptor.h"
#include "Global.h"
#include "VGA.h"
#include "LED.h"
#include "Button.h"
#include "Utils.h"
#include "Random.h"

#include "Graphics/Bitmap.h"
#include "Graphics/Drawing.h"
#include "Graphics/Font.h"

#include <stdint.h>
#include <string.h>

static uint32_t sqrti(uint32_t n);

extern Font OLFont;

void Epileptor(const char* message)
{
	uint8_t *framebuffer1=(uint8_t *)0x20000000;
	uint8_t *framebuffer2=(uint8_t *)0x20010000;
	memset(framebuffer1,0,320*200);
	memset(framebuffer2,0,320*200);

	for(int i=0;i<256;i++)
	{
		int r=ExtractRawRed(i);
		int g=ExtractRawGreen(i);
		int b=ExtractRawBlue(i);

		if(b && (r&1)==0 && (g&1)==0 ) b--;
		if(r) r--;
		if(g) g--;

		data.epileptor.replacements[i]=RawRGB(r,g,b);
	}

	SetVGAScreenMode320x200(framebuffer1);

	Bitmap frame1,frame2;
	InitializeBitmap(&frame1,320,200,320,framebuffer1);
	InitializeBitmap(&frame2,320,200,320,framebuffer2);

	int frame=0;
	int first=VGAFrameCounter();

	for(int i=0;i<NumberOfScrollerStars;i++)
	{
		data.epileptor.stars[i].x=Fix(RandomInteger()%320);
		data.epileptor.stars[i].y=RandomInteger()%200;

		int z=sqrti((NumberOfScrollerStars-1-i)*NumberOfScrollerStars)*1000/NumberOfScrollerStars;
		data.epileptor.stars[i].dx=6000*1200/(z+200);

		int c=255*i/NumberOfScrollerStars;

		switch(RandomInteger()%5)
		{
			case 0:
			case 1:
				data.epileptor.stars[i].c=RGB(0,c,c);
			break;

			case 2:
				data.epileptor.stars[i].c=RGB(c,c,c);
			break;

			case 3:
			case 4:
				data.epileptor.stars[i].c=RGB(c,0,c);
			break;
		}
	}

	while(CurrentBitBinRow(&song) < 1280)
	{
		WaitVBL();

		Bitmap *currframe;
		if(frame)
		{
			currframe=&frame2;
			SetFrameBuffer(framebuffer1);

			for(int j=0;j<320*200;j++)
			framebuffer2[j]=data.epileptor.replacements[framebuffer1[j]];
		}
		else
		{
			currframe=&frame1;
			SetFrameBuffer(framebuffer2);

			for(int j=0;j<320*200;j++)
			framebuffer1[j]=data.epileptor.replacements[framebuffer2[j]];
		}

		int t=VGAFrameCounter()-first;
		int x=320-t*2;

		for(int i=0;i<NumberOfScrollerStars;i++)
		{
			int oldx=data.epileptor.stars[i].x;
			int newx=oldx+data.epileptor.stars[i].dx;
			data.epileptor.stars[i].x=newx;

			DrawHorizontalLine(currframe,FixedToInt(oldx),data.epileptor.stars[i].y,
			FixedToInt(newx)-FixedToInt(oldx),data.epileptor.stars[i].c);

			if(newx>=Fix(320))
			{
				data.epileptor.stars[i].x=Fix(0);
				data.epileptor.stars[i].y=RandomInteger()%200;
			}
		}

		for(const char *ptr=message;*ptr;ptr++)
		{
			int c=*ptr;

			int y=(isin(x*20+t*33)>>7)+100-8;

			DrawCharacter(currframe,&OLFont,x,y,0,c);

			x+=WidthOfCharacter(&OLFont,c)+2;
		}

		SetLEDs(1<<((t/3)&3));

		frame^=1;
	}


	while(UserButtonState());
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
