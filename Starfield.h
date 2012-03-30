#ifndef __STARFIELD_H__
#define __STARFIELD_H__

#include <stdint.h>

#define MaxStarZ 120
#define MinStarZ 3
#define StarWidth 10000

#define NumberOfStars 1500

struct StarfieldData
{
	struct Star
	{
		int32_t x,y,z,c;
	} stars[NumberOfStars];

	int frames[MaxStarZ+1];
};

void Starfield();

#endif
