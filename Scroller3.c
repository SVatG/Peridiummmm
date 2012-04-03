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
#include "Graphics/RLEBitmap.h"

#include <stdint.h>
#include <string.h>

static uint32_t sqrti(uint32_t n);

extern Font OLFont;
extern const RLEBitmap smalllogo;

void Scroller3(const char *message)
{
	uint8_t *framebuffer1=(uint8_t *)0x20000000;
	uint8_t *framebuffer2=(uint8_t *)0x20010000;
	memset(framebuffer1,0,320*200);
	memset(framebuffer2,0,320*200);
	
	for(int32_t c = 0; c < 8; c++) {
		data.scroller2.barTable[0][c] = RawRGB(c,0,c/2);
		data.scroller2.barTable[1][c] = RawRGB(0,c,c/2);
		data.scroller2.barTable[2][c] = RawRGB(c,c,c/2);
		data.scroller2.barTable[0][15 - c] = RawRGB(c,0,c/2);
		data.scroller2.barTable[1][15 - c] = RawRGB(0,c,c/2);
		data.scroller2.barTable[2][15 - c] = RawRGB(c,c,c/2);
	}

	SetVGAScreenMode320x200(framebuffer1);

	Bitmap frame1,frame2;
	InitializeBitmap(&frame1,320,200,320,framebuffer1);
	InitializeBitmap(&frame2,320,200,320,framebuffer2);

	int frame=0;
	int first=VGAFrameCounter();
	
	for(int c = 0; c < 42; c++ ) {
		data.scroller3.circles[c] = IntToFixed(((c + 1) * 10))/28;
	}
	
	while(CurrentBitBinRow(&song) < 256)
	{
		WaitVBL();

		Bitmap *currframe;
		if(frame)
		{
			currframe=&frame2;
			SetFrameBuffer(framebuffer1);
		}
		else
		{
			currframe=&frame1;
			SetFrameBuffer(framebuffer2);
		}
		ClearBitmap(currframe);

		uint8_t* pixels = currframe->pixels;
		
		int t=VGAFrameCounter()-first;
		int x=320-(t*3);
		
		uint8_t cols[] = {
			RawRGB(7,0,2),
			RawRGB(0,7,2),
			RawRGB(7,7,3),
		};
		
		for(int c = 0; c < 42; c++ ) {
			data.scroller3.circles[c]-=256;
			if(data.scroller3.circles[c] <= 0) {
				data.scroller3.circles[c] = IntToFixed(15);
			}
			int32_t cval = (IntToFixed(t)>>8) + (data.scroller3.circles[c]>>5);
			DrawCircle(
				currframe,
				imul(isin(cval),data.scroller3.circles[c]>>8)+160,
				imul(icos(cval),data.scroller3.circles[c]>>8)+100,
				FixedToRoundedInt(idiv(IntToFixed(210),data.scroller3.circles[c])),
				cols[c%3]
			);
		}


		int y=(isin((t*30))>>7)+100-8;

		DrawHorizontalLine(currframe, 0, y-3, 320, RawRGB(7,7,3));
		for(int yd = -2; yd < 17; yd++) {
			DrawHorizontalLine(currframe, 0, y+yd, 320, RawRGB(0,0,0));
		}
		DrawHorizontalLine(currframe, 0, y+17, 320, RawRGB(7,7,3));

		
		for(const char *ptr=message;*ptr;ptr++)
		{
			int c=*ptr;
			DrawCharacter(currframe,&OLFont,x,y,0,c);
			x+=WidthOfCharacter(&OLFont,c)+2;
		}

		DrawRLEBitmap(currframe,&smalllogo,195,y-17);
		
		SetLEDs(1<<((t/3)&3));

		frame^=1;
	}


// 	while(UserButtonState());
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
