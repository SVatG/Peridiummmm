#ifndef __FONTHEADER_H_
#define __FONTHEADER_H_

#define GLYPHGROUPS 32

#include <stdint.h>

typedef struct {
    int x;
    int y;
} point_t;
        
typedef union {
    int i[6];
    point_t p[3];
} bezier_t;

typedef struct {
    unsigned char c;
    int width;
    const bezier_t* data;
    int datalen;
    uint16_t groupstart[GLYPHGROUPS];
    uint16_t grouplen[GLYPHGROUPS];
    int pathlen[GLYPHGROUPS];
} glyph_t;

typedef struct {
    const bezier_t* data;
    uint8_t datalen; 
} path_t;





#endif //__FONTHEADER_H_
