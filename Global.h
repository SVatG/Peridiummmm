#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "Epileptor.h"
#include "Scroller.h"
#include "Starfield.h"
#include "Rotozoomer.h"
#include "Metablobs.h"
#include "Rasterize.h"
#include "SpanScreen.h"
#include "LogoShow.h"
#include "Scroller2.h"
#include "Scroller3.h"
#include "Radial.h"
#include "BitBin.h"
#include "Greets.h"

extern BitBinSong song;

extern union GlobalData
{
	struct EpileptorData epileptor;
	struct ScrollerData scroller;
	struct StarfieldData starfield;
	struct RotozoomerData rotozoomer;
	struct MetablobsData metablobs;
	struct RasterizeData rasterizer;
	struct SpanScreenData spanscreen;
	struct LogoShowData logoshow;
	struct Scroller2Data scroller2;
	struct Scroller3Data scroller3;
	struct RadialData radial;
    struct GreetsData greets;
} data;

#endif
