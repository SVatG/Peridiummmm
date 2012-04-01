#ifndef __2DVECTOR_H_
#define __2DVECTOR_H_

/*
 * tiny 2d vector library (for bezier curves)
 */

#include "bezier.h" // for BEZ_STEPS
#include "fontheader.h" // for point_t
#include "Utils.h" // for sqrti

#include <stdlib.h>

inline point_t padd(const point_t a, const point_t b){
    point_t r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    return(r);
}

inline point_t psub(const point_t a, const point_t b){
    point_t r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    return(r);
}

inline point_t pscale(const point_t a, const int mul, const int shift){
    point_t r;
    r.x = (a.x * mul)>>shift;
    r.y = (a.y * mul)>>shift;
    return(r);
}

inline point_t pscale_bez(const point_t a, const int t){
    point_t r;
    r.x = (a.x * t)/BEZ_STEPS;
    r.y = (a.y * t)/BEZ_STEPS;
    return(r);
}

inline point_t pdiv(const point_t a, const int div){
    point_t r;
    r.x = a.x/div;
    r.y = a.y/div;
    return r;
}

inline point_t pavg(const point_t a, const point_t b){
    point_t r;
    r.x = (a.x + b.x)/2;
    r.y = (a.y + b.y)/2;
    return(r);
}

inline int p_inrect(const point_t a, const point_t r1, const point_t r2){
    int r = 1;
    if(a.x < r1.x || a.x > r2.x)
        r = 0;
    if(a.y < r1.y || a.y > r2.y)
        r = 0;
    return(r);
}

inline int plen(const point_t a, const point_t b){
    int r;
//    r = sqrti((b.x-a.x)*(b.x-a.x));
//    r += sqrti((b.y-a.y)*(b.y-a.y));
//  TODO: use fake-sqrt or something, not manhattan distance
    r = abs(b.x-a.x);
    r += abs(b.y-a.y);
    return(r);
}


inline int min(int a, int b){
   return((a<b)?a:b);
}
inline int max(int a, int b){
   return((a>b)?a:b);
}
#endif //__2DVECTOR_H_
