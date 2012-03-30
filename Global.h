#include "Scroller.h"
#include "Starfield.h"
#include "Rotozoomer.h"

extern union GlobalData
{
	struct ScrollerData scroller;
	struct StarfieldData starfield;
	struct RotozoomerData rotozoomer;
} data;
