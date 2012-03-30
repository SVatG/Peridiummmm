#include "Starfield.h"
#include "Global.h"
#include "VGA.h"
#include "Sprites.h"
#include "LED.h"
#include "Utils.h"
#include "Button.h"
#include "Random.h"

#include "Graphics/Bitmap.h"
#include "Graphics/Drawing.h"

#include <stdint.h>
#include <string.h>

void Starfield()
{
	uint8_t *framebuffer1=(uint8_t *)0x20000000;
	uint8_t *framebuffer2=(uint8_t *)0x20010000;
	memset(framebuffer1,0,320*200);
	memset(framebuffer2,0,320*200);

	SetVGAScreenMode320x200(framebuffer1);

	static const RLEBitmap *sprites[7*6]={
		&Star1_0,&Star2_0,&Star3_0,&Star4_0,&Star5_0,&Star6_0,&Star7_0,
		&Star1_1,&Star2_1,&Star3_1,&Star4_1,&Star5_1,&Star6_1,&Star7_1,
		&Star1_2,&Star2_2,&Star3_2,&Star4_2,&Star5_2,&Star6_2,&Star7_2,
		&Star1_3,&Star2_3,&Star3_3,&Star4_3,&Star5_3,&Star6_3,&Star7_3,
		&Star1_4,&Star2_4,&Star3_4,&Star4_4,&Star5_4,&Star6_4,&Star7_4,
		&Star1_5,&Star2_5,&Star3_5,&Star4_5,&Star5_5,&Star6_5,&Star7_5,
	};

	Bitmap frame1,frame2;
	InitializeBitmap(&frame1,320,200,320,framebuffer1);
	InitializeBitmap(&frame2,320,200,320,framebuffer2);

	for(int i=0;i<NumberOfStars;i++)
	{
		data.starfield.stars[i].x=RandomInteger()%(2*StarWidth)-StarWidth;
		data.starfield.stars[i].y=RandomInteger()%(2*StarWidth)-StarWidth;
		data.starfield.stars[i].z=i*(MaxStarZ-MinStarZ)/NumberOfStars+MinStarZ;

		data.starfield.stars[i].c=(RandomInteger()%6)*7;
	}

	for(int z=MinStarZ;z<=MaxStarZ;z++)
	{
		int frame=MaxStarZ/z-1;
		if(frame>6) frame=6;
		data.starfield.frames[z]=frame;
	}

	int frame=0;
	int nearestindex=0;
	int first=VGAFrameCounter();

	while(!UserButtonState())
	{
		WaitVBL();

		Bitmap *currframe;
		if(frame&1) { currframe=&frame2; SetFrameBuffer(framebuffer1); }
		else { currframe=&frame1; SetFrameBuffer(framebuffer2); }

		ClearBitmap(currframe);

		int t=VGAFrameCounter()-first;

		while(data.starfield.stars[nearestindex].z==MinStarZ)
		{
			data.starfield.stars[nearestindex].z=MaxStarZ;
			data.starfield.stars[nearestindex].x=RandomInteger()%(2*StarWidth)-StarWidth;
			data.starfield.stars[nearestindex].y=RandomInteger()%(2*StarWidth)-StarWidth;
			nearestindex++;
			if(nearestindex==NumberOfStars) nearestindex=0;
		}

		for(int n=NumberOfStars-1;n>=0;n--)
		{
			int i=n+nearestindex;
			if(i>=NumberOfStars) i-=NumberOfStars;

			int32_t sx=data.starfield.stars[i].x/data.starfield.stars[i].z+160;
			int32_t sy=data.starfield.stars[i].y/data.starfield.stars[i].z+100;
			int sprite=data.starfield.stars[i].c+data.starfield.frames[data.starfield.stars[i].z];
			DrawRLEBitmap(currframe,sprites[sprite],sx-16,sy-16);
		}

		int rotation=(t/40)%3-1;

		switch(rotation)
		{
			case -1:
				for(int i=0;i<NumberOfStars;i++)
				{
					data.starfield.stars[i].x+=data.starfield.stars[i].y>>6;
					data.starfield.stars[i].y-=data.starfield.stars[i].x>>6;
					data.starfield.stars[i].z--;
				}
			break;

			default:
			case 0:
				for(int i=0;i<NumberOfStars;i++)
				{
					data.starfield.stars[i].z--;
				}
			break;

			case 1:
				for(int i=0;i<NumberOfStars;i++)
				{
					data.starfield.stars[i].x-=data.starfield.stars[i].y>>6;
					data.starfield.stars[i].y+=data.starfield.stars[i].x>>6;
					data.starfield.stars[i].z--;
				}
			break;
		}

		frame++;
	}

	while(UserButtonState());
}
