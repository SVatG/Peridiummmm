#ifndef __RADIAL_H__
#define __RADIAL_H__

#include <stdint.h>

struct RadialData
{
	uint32_t dummy;
	uint8_t frame[212*132];
};

void RadialScroller(const char *text);

#endif
