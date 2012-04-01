#include "bezier.h"
#include <stdlib.h>
#include <stdio.h>

#define WIDTH 320
#define HEIGHT 200


void bezier_draw_path_partial(Bitmap *dest, bezier_t *data, int len, int percent){
    for(int i=0; i<len; ++i){
        point_t a,b,p;
        int x,y;
        point_t* bez = (point_t*) (data + i);
        for(int t=0; t<BEZ_STEPS; ++t){
            a = padd(bez[0], pscale_bez(psub(bez[1],bez[0]),t));
            b = padd(bez[1], pscale_bez(psub(bez[2],bez[1]),t));
            p = padd(a, pscale_bez(psub(b,a),t));
            bezier_setpixel(dest, p,255);

        }
#ifdef SHOW_CONTROL_POINTS        
        bezier_setpixel(dest, bez.p[2],7<<5);
        bezier_setpixel(dest, bez.p[1],7<<2);
#endif //SHOW_CONTROL_POINTS
    }
}

void bezier_draw_partial(Bitmap *dest, bezier_t bez, int percent){
    point_t a,b,p;
    int x,y;
    // clamp
    if(percent>=200){
        percent=199;
    }
    if(percent<0){
        percent=0;
    }
    if(percent<100){
        // render forward
        for(int t=0; t<(BEZ_STEPS*percent/100); ++t){
            a = padd(bez.p[0], pscale_bez(psub(bez.p[1],bez.p[0]),t));
            b = padd(bez.p[1], pscale_bez(psub(bez.p[2],bez.p[1]),t));
            p = padd(a, pscale_bez(psub(b,a),t));
            bezier_setpixel(dest, p,255);
        }
    } else {
        // render backwards
        for(int t=(BEZ_STEPS*(percent-99)/100); t<BEZ_STEPS; ++t){
            a = padd(bez.p[0], pscale_bez(psub(bez.p[1],bez.p[0]),t));
            b = padd(bez.p[1], pscale_bez(psub(bez.p[2],bez.p[1]),t));
            p = padd(a, pscale_bez(psub(b,a),t));
            bezier_setpixel(dest, p,255);
        }
    }
#ifdef DEBUG
    callcount_bezier++;
#endif //DEBUG
}

void bezier_draw(Bitmap *dest, bezier_t bez){
    point_t a,b,p;
    int x,y;
    for(int t=0; t<BEZ_STEPS; ++t){
        a = padd(bez.p[0], pscale_bez(psub(bez.p[1],bez.p[0]),t));
        b = padd(bez.p[1], pscale_bez(psub(bez.p[2],bez.p[1]),t));
        p = padd(a, pscale_bez(psub(b,a),t));
        bezier_setpixel(dest, p,255);
    }
#ifdef SHOW_CONTROL_POINTS        
    bezier_setpixel(dest, bez[2],7<<5);
    bezier_setpixel(dest, bez[1],7<<2);
#endif //SHOW_CONTROL_POINTS
#ifdef DEBUG
    callcount_bezier++;
#endif //DEBUG
}

void bezier_draw_improved(Bitmap *dest, bezier_t bez){
    static bezier_t stack[10];
    int stackpos = 0;
    stack[0] = bez;
    //TODO: draw last point of curve

    while(stackpos >=0){
        bezier_t b = stack[stackpos--];
        int len = plen(b.p[0],b.p[1])+plen(b.p[1],b.p[2]);
        if(len>BEZ_SCALEDOWN){
            point_t p1, p2, pm;
            p1 = pavg(b.p[0], b.p[1]);
            p2 = pavg(b.p[1], b.p[2]);
            pm = pavg(p1,p2);
            bezier_t t;
            t.p[0] = b.p[0];
            t.p[1] = p1;
            t.p[2] = pm;
            stack[++stackpos] = t;
            t.p[0] = pm;
            t.p[1] = p2;
            t.p[2] = b.p[2];
            stack[++stackpos] = t;
        } else {
            bezier_setpixel(dest, b.p[0], 255);
        }
    }
}


void bezier_setpixel(Bitmap *dest, point_t p, uint8_t colour){
    int x,y, distance;
    x = p.x / BEZ_SCALEDOWN;
    y = p.y / BEZ_SCALEDOWN;
    if(x>=WIDTH || x<0)
        return;
    if(y>=HEIGHT || y<0)
        return;
    distance = abs((p.x % BEZ_SCALEDOWN)-BEZ_SCALEDOWN/2);
    distance += abs((p.y % BEZ_SCALEDOWN)-BEZ_SCALEDOWN/2);
/*    static int ctr;
    ctr++;
    if(ctr>10000){
        printf("%i\n",distance);
        ctr=0;
    }*/
    int intensity = 7-(distance * 4) / BEZ_SCALEDOWN;
    if(intensity<0)
        intensity=0;
    
//    image[p.x+(p.y)*WIDTH]=colour;
    colour = (intensity<<5) + (intensity<<2) + (intensity>>1);
    int oldpixel = dest->pixels[x+(HEIGHT-y-1)*WIDTH];
    int oldintensity = (oldpixel>>5)&7;
    if(oldintensity<intensity){
        dest->pixels[x+(HEIGHT-y-1)*WIDTH] = colour;
    }
#ifdef DEBUG
    callcount_setpixel++;
#endif //DEBUG

}

/*int bezier_len(bezier_t b){
   int r = pdist(p.b[0],b.p[1]);
   r += pdist(p.b[1],b.p[2]);
   return(r);
}*/



