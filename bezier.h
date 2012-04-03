#ifndef __BEZIER_H_
#define __BEZIER_H_

#define BEZ_SCALEDOWN 1024 
#define BEZ_STEPS 128

//#include "main.h"
#include "fontheader.h"
#include "2dvector.h"

#include "Graphics/Bitmap.h"


//void bezier_draw_path(Bitmap *dest, bezier_t *data, int len);
void bezier_draw(Bitmap *dest, bezier_t bez);
void bezier_draw_partial(Bitmap *dest, bezier_t bez, int percent);

void bezier_setpixel(Bitmap *dest, point_t p, uint8_t colour);


#endif // __BEZIER_H_
