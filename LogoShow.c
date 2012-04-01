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

	while(!UserButtonState())
	{
		WaitVBL();

		Bitmap *currframe;
		if(t&1)
		{
			SetFrameBuffer(framebuffer1);
			ClearBitmap(&frame2);
			currframe=&frame2;
		}
		else
		{
			SetFrameBuffer(framebuffer2);
			ClearBitmap(&frame1);
			currframe=&frame1;
		}

		SetLEDs(0);

		uint8_t* pixels = currframe->pixels;

		if(t < 600) {
			for(int y = 0; y < 200; y++ ) {
				for(int x = 0; x < 320; x++) {
					int16_t shval  = (100 - ((2*y - x) / 12)) - (t - 100)/4;
					shval = shval < 0 ? 0 : shval;
					pixels[x+y*320] =
						(Logo_0[x+y*320] & 0xE0 >> shval) & 0xE0 |
						(Logo_0[x+y*320] & 0x1C >> shval) & 0x1C |
						(Logo_0[x+y*320] & 0x03 >> shval) & 0x03;
				}
			}
		}
		else {
			for(int y = 0; y < 200; y++ ) {
				for(int x = 0; x < 320; x++) {
					int16_t shval  = 8 - ((100 - ((2*y - x) / 12)) - (t - 900)/4);
					shval = shval < 0 ? 0 : shval;
					pixels[x+y*320] =
						(Logo_0[x+y*320] & 0xE0 >> shval) & 0xE0 |
						(Logo_0[x+y*320] & 0x1C >> shval) & 0x1C |
						(Logo_0[x+y*320] & 0x03 >> shval) & 0x03;
				}
			}
		}
		t++;
	}

	while(UserButtonState());
}