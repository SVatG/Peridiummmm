#ifndef __GREETS_H
#define __GREETS_H

#include "font.h"
#include "Graphics/Bitmap.h"
#include "greetstars.h"


typedef struct{
    point_t pos;
    char* text;
    int size;
    warpfunc_t warpfunc; 
//    warpstruct_t warpparams;
    bool show;
} greet_t;

struct GreetsData
{
    greetstars_t stars_a[GREETSTARS_COUNT];
    greetstars_t stars_b[GREETSTARS_COUNT];
    point_t textpts1[200];
    point_t textpts2[200];
};
        

void greets_init();
void greets_next_effect();
void greets_inner(Bitmap* currframe);
void logo_inner(Bitmap* currframe);
void Greets();
void RevisionLogo();
void print_vga_line(Bitmap* currframe);

bool isdone();

#endif //__GREETS_H
