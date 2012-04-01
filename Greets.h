#ifndef __GREETS_H
#define __GREETS_H

#include "font.h"
#include "Graphics/Bitmap.h"

typedef struct{
    point_t pos;
    char* text;
    int size;
    bool show;
} greet_t;
        

void greets_init();
void greets_inner(Bitmap* currframe);
void Greets();

bool isdone();

#endif //__GREETS_H
