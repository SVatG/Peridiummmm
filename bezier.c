//#include "main.h"
#include "bezier.h"
#include <stdlib.h>
#include <stdio.h>

#define WIDTH 320
#define HEIGHT 200
/*
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
}*/

void bezier_draw_partial(Bitmap *dest, bezier_t bez, int ratio){    
    // ratio is between 0 and 128. Or negative for drawing only the end, not the beginning.
    static bezier_t stack[15]; //TODO: merge with stack from bezier_draw
    static int ratios[15];
    int stackpos = 0;
    if(ratio<0){ 
        // turn around bez
        ratio = -ratio;
        point_t tmp;
        tmp = bez.p[0];
        bez.p[0] = bez.p[2];
        bez.p[2] = tmp;
    }

    stack[0] = bez;
    ratios[0] = ratio;

    while(stackpos >=0){
        int r = ratios[stackpos];
        bezier_t *b = &stack[stackpos--];
        if(r<=0){
            continue; // don't draw this sub-bezier
        }
        int len = plen(b->p[0],b->p[1])+plen(b->p[1],b->p[2]);
        if(len>BEZ_SCALEDOWN*2){
            point_t p1, p2, pm, bp2;
            p1 = pavg(b->p[0], b->p[1]);
            p2 = pavg(b->p[1], b->p[2]);
            pm = pavg(p1,p2);
            bezier_t *t;
            t = &stack[++stackpos];
            t->p[0] = b->p[0];
            t->p[1] = p1;
            bp2 = b->p[2]; // rescue value before writing there
            t->p[2] = pm;
            ratios[stackpos] = r*2;
            t = &stack[++stackpos];
            t->p[0] = pm;
            t->p[1] = p2;
            t->p[2] = bp2;
            ratios[stackpos] = r*2-128;
        } else {
            bezier_setpixel(dest, b->p[0], 255);//182);
            if(r>=64){
                bezier_setpixel(dest, b->p[1], 255);//182);
            }
        }
    }
}


void bezier_draw(Bitmap *dest, bezier_t bez){
    static bezier_t stack[15];
    int stackpos = 0;
    stack[0] = bez;
    //TODO: draw last point of curve

    while(stackpos >=0){
        bezier_t *b = &stack[stackpos--];
        int len = plen(b->p[0],b->p[1])+plen(b->p[1],b->p[2]);
        if(len>BEZ_SCALEDOWN*2){
            point_t p1, p2, pm, bp2;
            p1 = pavg(b->p[0], b->p[1]);
            p2 = pavg(b->p[1], b->p[2]);
            pm = pavg(p1,p2);
            bezier_t *t;
            t = &stack[++stackpos];
            t->p[0] = b->p[0];
            t->p[1] = p1;
            bp2 = b->p[2]; // rescue value before writing there
            t->p[2] = pm;
            t = &stack[++stackpos];
            t->p[0] = pm;
            t->p[1] = p2;
            t->p[2] = bp2;
        } else {
            bezier_setpixel(dest, b->p[0], 255);
            bezier_setpixel(dest, b->p[1], 255);
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
    int intensity = 9-(distance * 4) / BEZ_SCALEDOWN;
    if(intensity<0)
        intensity=0;
    if(intensity>8)
        intensity=8;
    
//    image[p.x+(p.y)*WIDTH]=colour;
    uint8_t oldpixel = dest->pixels[x+(HEIGHT-y-1)*WIDTH];
    int colour_decomposed = (colour * ((1<<(8+16))|(1<<(6+8))|(1<<3))) & 0x03070700;
    int oldpixel_decomposed = (oldpixel * ((1<<(8+16))|(1<<(6+8))|(1<<3))) & 0x03070700;
    int colour_scaled = (colour_decomposed*intensity+0x03010100)>>3;
    int newcolour = (colour_scaled) & 0x03070700; //FIXME: add oldpixel here or something
    int newcolour2 = ((newcolour * ((1)|(1<<(2+8))|(1<<(5+16)))) & 0xff000000)>>24;
//    printf("%x %x %x %x\n",colour, newcolour, colour_scaled, newcolour);
    dest->pixels[x+(HEIGHT-y-1)*WIDTH] = newcolour2;
#ifdef DEBUG
    callcount_setpixel++;
#endif //DEBUG
}

/*int bezier_len(bezier_t b){
   int r = pdist(p.b[0],b.p[1]);
   r += pdist(p.b[1],b.p[2]);
   return(r);
}*/



