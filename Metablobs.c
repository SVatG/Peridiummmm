#include "Starfield.h"
#include "VGA.h"
#include "LED.h"
#include "Button.h"
#include "Random.h"

#include "Graphics/Bitmap.h"
#include "Graphics/RLEBitmap.h"
#include "Graphics/Drawing.h"

#include "VectorLibrary/VectorFixed.h"
#include "VectorLibrary/MatrixFixed.h"
#define Viewport(x,w,s) (imul(idiv((x),(w))+IntToFixed(1),IntToFixed((s)/2)))

#include <stdint.h>
#include <string.h>

#include "Blobs.h"

#include "Utils.h"


#include "Graphics/RLEBitmap.h"

#include "Metablobs.h"
#include "Global.h"

#include "BitBin.h"

extern const RLEBitmap EndLogo;

void Metablobs()
{
	uint8_t *framebuffer1=(uint8_t *)0x20000000;
	uint8_t *framebuffer2=(uint8_t *)0x20010000;
	memset(framebuffer1,0,320*200);
	memset(framebuffer2,0,320*200);

	const uint8_t *blobImg[6]={
		BLOB1,
		BLOB2,
		BLOB3,
		BLOB4,
		BLOB5,
		BLOB6,
	};
	
	Bitmap frame1,frame2;
	InitializeBitmap(&frame1,320,200,320,framebuffer1);
	InitializeBitmap(&frame2,320,200,320,framebuffer2);

	SetVGAScreenMode320x200(framebuffer1);
	
	int frame=0;
	
	for( int i = 0; i < numBlobs; i++ ) {
		data.metablobs.blobs[i].x = (RandomInteger()%300) + 10;
		data.metablobs.blobs[i].y = (RandomInteger()%180) + 10;
		data.metablobs.blobs[i].blob = blobImg[RandomInteger()%6];
	}

	while(CurrentBitBinRow(&song) < 1856)
	{
		WaitVBL();
		
		Bitmap *currframe;
		if(frame&1) { currframe=&frame2; SetFrameBuffer(framebuffer1); }
		else { currframe=&frame1; SetFrameBuffer(framebuffer2); }

		ClearBitmap(currframe);
		int movementMode = (CurrentBitBinRow(&song)>>4) % 3;
		for( int i = 0; i < numBlobs; i++ ) {
			if(movementMode == 0) {
				data.metablobs.blobs[i].x += 5 - ((blobImg[0] - data.metablobs.blobs[i].blob) / (19*19)*2);
				data.metablobs.blobs[i].y += (160 - data.metablobs.blobs[i].x) / 20;
			}
			if(movementMode == 1) {
				data.metablobs.blobs[i].x += 5 - ((blobImg[0] - data.metablobs.blobs[i].blob) / (19*19)*2);
				data.metablobs.blobs[i].y -= (160 - data.metablobs.blobs[i].x) / 20;
			}
			if(movementMode == 2) {
				data.metablobs.blobs[i].x -= 5 - ((blobImg[0] - data.metablobs.blobs[i].blob) / (19*19)*2);
				data.metablobs.blobs[i].y -= (160 - data.metablobs.blobs[i].x) / 20;
			}
			if(data.metablobs.blobs[i].x >= 330) {
				data.metablobs.blobs[i].x = -9;
				data.metablobs.blobs[i].y = (RandomInteger()%180) + 10;
			}
			if(data.metablobs.blobs[i].x <= -10) {
				data.metablobs.blobs[i].x = 329;
				data.metablobs.blobs[i].y = (RandomInteger()%180) + 10;
			}
			if(data.metablobs.blobs[i].y <= -10) {
				data.metablobs.blobs[i].y = 209;
				data.metablobs.blobs[i].x = (RandomInteger()%300) + 10;
			}
			if(data.metablobs.blobs[i].y >= 210) {
				data.metablobs.blobs[i].y = -9;
				data.metablobs.blobs[i].x = (RandomInteger()%300) + 10;
			}
		}
		
		uint8_t* pixels = currframe->pixels;
		
		for( int i = 0; i < numBlobs; i++ ) {
			for( int x = 0; x < 19; x++ ) {
				for( int y = 0; y < 19; y++ ) {
					int fx = data.metablobs.blobs[i].x+x-9;
					int fy = data.metablobs.blobs[i].y+y-9;
					if(fx < 0 || fx >= 320 || fy < 0 || fy >= 200) continue;
					int idx = fx+fy*320;
					uint32_t things = pixels[idx];
					things += data.metablobs.blobs[i].blob[x+y*19];
					things = things > 0xFF ? 0xFF : things;
					pixels[idx] = things;;
				}
			}
		}

		if( CurrentBitBinRow(&song) < 1824 ) {
			for( int x = 0; x < 320; x++ ) {
				for( int y = 0; y < 200; y++ ) {
					pixels[x+y*320] = (pixels[x+y*320] & 0xE0) | (pixels[x+y*320] >> 6 & 0x3);
				}
			}
		}
		else {
			int pixval = 0;
			for( int x = 0; x < 320; x++ ) {
				for( int y = 0; y < 200; y++ ) {
					pixval = FixedToInt(imul(IntToFixed(pixels[x+y*320]), IntToFixed(1855 - CurrentBitBinRow(&song))/32));
					pixels[x+y*320] = (pixval & 0xE0) | (pixval >> 6 & 0x3);
				}
			}
		}

		DrawRLEBitmap(currframe, &EndLogo, 10, 100 - 31);
		
		frame++;
	}

	// End of demo. Hang.
	while(1) {
		WaitVBL();
	}
}

