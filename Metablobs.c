#include "Starfield.h"
#include "VGA.h"
#include "LED.h"
#include "Button.h"
#include "Random.h"

#include "Graphics/Bitmap.h"
#include "VectorLibrary/VectorFixed.h"
#include "VectorLibrary/MatrixFixed.h"
#define Viewport(x,w,s) (imul(idiv((x),(w))+IntToFixed(1),IntToFixed((s)/2)))

#include <stdint.h>
#include <string.h>

#include "Blobs.h"

#include "Utils.h"

#define numBlobs 40

#include "Graphics/RLEBitmap.h"

extern const RLEBitmap Hakkero;

typedef struct {
	int32_t x;
	int32_t y;
	const uint8_t* blob;
} Blob;
Blob blobs[numBlobs];

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
		blobs[i].x = (RandomInteger()%300) + 10;
		blobs[i].y = (RandomInteger()%180) + 10;
		blobs[i].blob = blobImg[RandomInteger()%6];
	}

	while(!UserButtonState())
	{
		WaitVBL();
		
		Bitmap *currframe;
		if(frame&1) { currframe=&frame2; SetFrameBuffer(framebuffer1); }
		else { currframe=&frame1; SetFrameBuffer(framebuffer2); }

		ClearBitmap(currframe);
		int movementMode = (frame/100) % 3;
		for( int i = 0; i < numBlobs; i++ ) {
			if(movementMode == 0) {
				blobs[i].x += 5 - ((blobImg[0] - blobs[i].blob) / (19*19)*2);
				blobs[i].y += (160 - blobs[i].x) / 20;
			}
			if(movementMode == 1) {
				blobs[i].x += 5 - ((blobImg[0] - blobs[i].blob) / (19*19)*2);
				blobs[i].y -= (160 - blobs[i].x) / 20;
			}
			if(movementMode == 2) {
				blobs[i].x -= 5 - ((blobImg[0] - blobs[i].blob) / (19*19)*2);
				blobs[i].y -= (160 - blobs[i].x) / 20;
			}
			if(blobs[i].x >= 330) {
				blobs[i].x = -9;
				blobs[i].y = (RandomInteger()%180) + 10;
			}
			if(blobs[i].x <= -10) {
				blobs[i].x = 329;
				blobs[i].y = (RandomInteger()%180) + 10;
			}
			if(blobs[i].y <= -10) {
				blobs[i].y = 209;
				blobs[i].x = (RandomInteger()%300) + 10;
			}
			if(blobs[i].y >= 210) {
				blobs[i].y = -9;
				blobs[i].x = (RandomInteger()%300) + 10;
			}
		}
		
		uint8_t* pixels = currframe->pixels;
		
		for( int i = 0; i < numBlobs; i++ ) {
			for( int x = 0; x < 19; x++ ) {
				for( int y = 0; y < 19; y++ ) {
					int fx = blobs[i].x+x-9;
					int fy = blobs[i].y+y-9;
					if(fx < 0 || fx >= 320 || fy < 0 || fy >= 200) continue;
					int idx = fx+fy*320;
					uint32_t things = pixels[idx];
					things += blobs[i].blob[x+y*19];
					things = things > 0xFF ? 0xFF : things;
					pixels[idx] = things;;
				}
			}
		}

		for( int x = 0; x < 320; x++ ) {
			for( int y = 0; y < 200; y++ ) {
				pixels[x+y*320] = (pixels[x+y*320] & 0xE0) | (pixels[x+y*320] >> 6 & 0x3);
			}
		}
		
		frame++;
	}

	while(UserButtonState());
}

