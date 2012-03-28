#include "Epileptor.h"
#include "VGA.h"
#include "LED.h"
#include "Button.h"
#include "Utils.h"

#include "Graphics/Bitmap.h"
#include "Graphics/Drawing.h"

#include <stdint.h>
#include <string.h>

static void DrawBlob(Bitmap *bitmap,int x0,int y0,int c);

void Epileptor()
{
	static uint8_t palette[18]=
	{
		RawRGB(7,0,0),RawRGB(7,2,0),RawRGB(7,5,0),
		RawRGB(7,7,0),RawRGB(5,7,0),RawRGB(2,7,0),
		RawRGB(0,7,0),RawRGB(0,7,1),RawRGB(0,7,2),
		RawRGB(0,7,3),RawRGB(0,5,3),RawRGB(0,2,3),
		RawRGB(0,0,3),RawRGB(2,0,3),RawRGB(5,0,3),
		RawRGB(7,0,3),RawRGB(7,0,2),RawRGB(7,0,1),
	};

	static uint8_t replacements[256];

	uint8_t *framebuffer1=(uint8_t *)0x20000000;
	uint8_t *framebuffer2=(uint8_t *)0x20010000;
	memset(framebuffer1,0,320*200);
	memset(framebuffer2,0,320*200);

	for(int i=0;i<sizeof(palette);i++)
	{
		replacements[palette[i]]=palette[(i+1)%sizeof(palette)];
	}

	SetVGAScreenMode320x200(framebuffer1);

	Bitmap frame1,frame2;
	InitializeBitmap(&frame1,320,200,320,framebuffer1);
	InitializeBitmap(&frame2,320,200,320,framebuffer2);

	int t=0;

	while(!UserButtonState())
	{
		WaitVBL();

		Bitmap *currframe;
		if(t&1)
		{
			currframe=&frame2;
			SetFrameBuffer(framebuffer1);

			for(int j=0;j<320*200;j++)
			framebuffer2[j]=replacements[framebuffer1[j]];
		}
		else
		{
			currframe=&frame1;
			SetFrameBuffer(framebuffer2);

			for(int j=0;j<320*200;j++)
			framebuffer1[j]=replacements[framebuffer2[j]];
		}

		SetLEDs(1<<((t/3)&3));

		int32_t sina=isin(t*3);
		int32_t sinb=isin(10*t/11);

		for(int j=0;j<4;j++)
		{
			int32_t sin1=isin(sina+j*1024);
			int32_t cos1=icos(sina+j*1024);
			int32_t sin2=isin(sinb/3+j*1024+1421);
			int32_t cos2=icos(sinb/3+j*1024+1421);
			int32_t sin3=isin(t*23);
			int32_t sin4=isin(t*14);

			int x=160+((imul(sin1,sin4)+imul(sin2,sin3)/2)>>5);
			int y=100+((imul(cos1,sin4)+imul(cos2,sin3)/2)>>5);

			DrawBlob(currframe,x,y,palette[0]);
		}

		t++;
	}

	while(UserButtonState());
}

static void DrawBlob(Bitmap *bitmap,int x0,int y0,int c)
{
	static const int rowlengths[32]=
	{
		6,12,16,20,  22,24,26,26, 28,28,30,30, 30,32,32,32,
		32,32,32,30, 30,30,28,28, 26,26,24,22, 20,16,12,6
	};

	for(int row=0;row<32;row++)
	DrawHorizontalLine(bitmap,x0-rowlengths[row]/2,y0+row-16,rowlengths[row],c);
}
