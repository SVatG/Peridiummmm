#ifndef __EPILEPTOR_H__
#define __EPILEPTOR_H__

#include <stdint.h>

struct EpileptorData
{
	uint8_t replacements[256];
};

void Epileptor();

#endif
