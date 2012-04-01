#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "Epileptor.h"
#include "Scroller.h"
#include "Starfield.h"
#include "Rotozoomer.h"
#include "Metablobs.h"
#include "Rasterize.h"
#include "SpanScreen.h"

extern union GlobalData
{
	struct EpileptorData epileptor;
	struct ScrollerData scroller;
	struct StarfieldData starfield;
	struct RotozoomerData rotozoomer;
	struct MetablobsData metablobs;
	struct RasterizeData rasterizer;
	struct SpanScreenData spanscreen;
} data;

#endif
