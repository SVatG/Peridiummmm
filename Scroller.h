#ifndef __SCROLLER_H__
#define __SCROLLER_H__

#include <stdint.h>

struct ScrollerData
{
	uint32_t scanline;

	struct
	{
		uint8_t texture;
		uint8_t dummy[3];
		uint32_t pos;
		uint32_t delta;
		int32_t z;
	} lines[400];

	uint8_t copper[6][16];
};

void Scroller(const char *text);

#endif
