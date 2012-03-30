#include "Scroller.h"
#include "Starfield.h"
#include "Rotozoomer.h"
#include "Metablobs.h"
#include "Rasterize.h"

extern union GlobalData
{
	struct ScrollerData scroller;
	struct StarfieldData starfield;
	struct RotozoomerData rotozoomer;
	struct MetablobsData metablobs;
	struct RasterizeData rasterizer;
} data;
