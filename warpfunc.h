#ifndef __WARPFUNC_H_
#define __WARPFUNC_H_


#include "bezier.h"

typedef point_t (*warpfunc_t)(point_t, int);


point_t warp_up_away(point_t a, int t);
point_t warp_perspective(point_t a, int t);


point_t warp_perspective_1(point_t a, int t);
point_t warp_perspective_2(point_t a, int t);
point_t warp_perspective_3(point_t a, int t);
point_t warp_perspective_4(point_t a, int t);
point_t warp_perspective_rotright(point_t a, int t);
point_t warp_perspective_5(point_t a, int t);
point_t warp_perspective_6(point_t a, int t);
point_t warp_perspective_7(point_t a, int t);
point_t warp_perspective_8(point_t a, int t);
point_t warp_perspective_inverted(point_t a, int t);
point_t warp_perspective_zoomin(point_t a, int t);
point_t warp_perspective_zoomout(point_t a, int t);







#endif //__WARPFUNC_H_
