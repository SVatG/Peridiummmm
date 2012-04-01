#ifndef __GREETS_H
#define __GREETS_H

#include "ryx/font.h"

typedef struct{
    point_t pos;
    char* text;
    bool show;
} greet_t;
        

void Greets();

#endif //__GREETS_H
