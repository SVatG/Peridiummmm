#ifndef __SCROLLER_H__
#define __SCROLLER_H__

#include <stdint.h>

struct ScrollerData
{
	struct
	{
		uint8_t texture;
		uint8_t dummy[3];
		uint32_t pos;
		uint32_t delta;
		int32_t z;
	} lines[400];
};

void Scroller(const char *text);

#endif
