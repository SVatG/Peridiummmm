#ifndef __SCROLLER2_H__
#define __SCROLLER2_H__

#include <stdint.h>

void Scroller2(const char *message);

struct Scroller2Data {
	uint8_t barTable[3][16];
};

#endif
