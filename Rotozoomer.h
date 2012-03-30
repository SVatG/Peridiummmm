#ifndef __ROTOZOOMER_H__
#define __ROTOZOOMER_H__

#include <stdint.h>

struct RotozoomerData
{
	volatile int32_t x0,y0,dx,dy;
	volatile uint32_t pos,delta;
	uint8_t texture[400];
};

void Rotozoom();

#endif
