/**
 * Show a bitmap from flash.
 */

#include "Global.h"
#include "VGA.h"
#include "Sprites.h"
#include "LED.h"
#include "Utils.h"
#include "Button.h"
#include "Random.h"

#include "Graphics/Bitmap.h"
#include "Graphics/Drawing.h"

#include <string.h>

extern const Pixel Logo_0[NumberOfPixelsForWidthAndHeight(320,200)];

void LogoShow() {
	uint8_t *framebuffer1=(uint8_t *)0x20000000;
	uint8_t *framebuffer2=(uint8_t *)0x20010000;

	Bitmap frame1,frame2;
	InitializeBitmap(&frame1,320,200,320,framebuffer1);
	InitializeBitmap(&frame2,320,200,320,framebuffer2);

	SetVGAScreenMode320x200(framebuffer1);

	int t=0;

	RasterizeInit();

	for(int32_t c = 0; c < 256; c++) {
		for(int32_t shval = 0; shval < 4; shval++) {
			data.logoshow.colourLut[c+shval*256] =
					(c & 0xE0 >> shval) & 0xE0 |
					(c & 0x1C >> shval) & 0x1C |
					(c & 0x03 >> shval) & 0x03;
		}
	}

	int32_t drawc = 0;
	while(!UserButtonState())
	{
		WaitVBL();
		Bitmap *currframe;
		if(t&1)
		{
			SetFrameBuffer(framebuffer1);
			currframe=&frame2;
		}
		else
		{
			SetFrameBuffer(framebuffer2);
			currframe=&frame1;
			drawc++;
		}
		
		uint8_t* pixels = currframe->pixels;
		if(t < 600) {
			int32_t tval = ((t - 100)>>2);
			for(int32_t y = (drawc&1)*100; y < ((drawc&1)+1)*100; y++ ) {
				int32_t yval = (y<<1);
				for(int32_t x = 0; x < 320; x++) {
					int32_t shval  = ((100 - ((yval - x) >> 4)) - tval);
					shval = shval < 0 ? 0 : shval;
					shval = shval > 3 ? 3 : shval;
					int32_t pos = x+y*320;
					pixels[pos] = data.logoshow.colourLut[Logo_0[pos]+shval*256];
				}
			}
		}
		else {
			if(t < 900) {
				memcpy(pixels,Logo_0,320*200);
			}
			else {
				int32_t tval = ((t - 900)>>2);
				for(int32_t y = (drawc&1)*100; y < ((drawc&1)+1)*100; y++ ) {
					int32_t yval = (y<<1);
					for(int32_t x = 0; x < 320; x++) {
						int32_t shval  = 8 - ((100 - ((yval - x) >> 4)) - tval);
						shval = shval < 0 ? 0 : shval;
						shval = shval > 3 ? 3 : shval;
						int32_t pos = x+y*320;
						pixels[pos] = data.logoshow.colourLut[Logo_0[pos]+shval*256];
					}
				}
			}
		}
		t++;
	}

	while(UserButtonState());
}