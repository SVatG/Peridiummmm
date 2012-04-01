#ifndef __SPAN_SCREEN_H__
#define __SPAN_SCREEN_H__

#include <stdint.h>

#define MaxSpans 8192

struct SpanScreenData
{
	uint16_t *timepointer;
	uint8_t *colourpointer;
	uint16_t times[2][MaxSpans];
	uint8_t colours[2][MaxSpans];
};

void SpanScreen();

#endif
