#ifndef __EPILEPTOR_H__
#define __EPILEPTOR_H__

#include <stdint.h>

#define NumberOfScrollerStars 2000

struct EpileptorData
{
	uint8_t replacements[256];

	struct
	{
		int32_t x,dx;
		uint8_t y,c;
	} stars[NumberOfScrollerStars];
};

void Epileptor(const char *message);

#endif
