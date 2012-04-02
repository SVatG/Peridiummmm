#include "Greets.h"

#include <string.h>

#include "Random.h"
#include "Graphics/Bitmap.h"
#include "Graphics/Drawing.h"

#ifndef TESTING
#include "Button.h"
#include "VGA.h"
#endif //TESTING

extern glyph_t font_enri_glyph[];
extern glyph_t revision_logo_glyph[];

greet_t greets[] = {
    { {0,0}, NULL, 0, false}, // filler
    // pos,     text,   size,   show 
	{ {40, 90}, "RNO", 130, true },
	{ {80, 30}, "nuance", 70, true },
	{ {20, 60}, "mercury", 70, true },
	{ {50, 20}, "never", 70, true },
	{ {20, 110}, "alpha design", 50, true },
	{ {60, 50}, "Stroboholics", 45, true },
	{ {20, 140}, "Marvin Minsky", 45, true },
	{ {40, 30}, "farbrausch", 55, true },
	{ {25, 70}, "#mod_shrine", 50, true },
	{ {30, 130}, "noisechanradio", 40, true },
	{ {20, 40}, "ubiktune", 70, true },
	{ {15, 70}, "fuckings 2 lamers", 40, true },
    { {0,0}, NULL, 0, false}, // filler
    { {0,0}, NULL, 0, false} // filler
};


int greetindex;
greet_t *gprev, *gcur, *gnext;
int tick,tock;
bool done = false;
point_t textpts1[200];
point_t textpts2[200];

void greets_init(){
    greetindex=1;
    gprev = greets+0;
    gcur = greets+1;
    gnext = greets+2;
    tick=-1;
    done = false;

}

void logo_init(){
    tick = -1;
    done = false;

}

#ifndef TESTING
void Greets(){
	uint8_t *framebuffer1=(uint8_t *)0x20000000;
	uint8_t *framebuffer2=(uint8_t *)0x20010000;
	memset(framebuffer1,0,320*200);
	memset(framebuffer2,0,320*200);

	SetVGAScreenMode320x200(framebuffer1);


    glyph_t glyph = font_enri_glyph[50];

	Bitmap frame1,frame2;
	InitializeBitmap(&frame1,320,200,320,framebuffer1);
	InitializeBitmap(&frame2,320,200,320,framebuffer2);
	Bitmap *currframe = &frame1;
    
    greets_init();

	while(!UserButtonState() && (!done))
	{
		WaitVBL();

        if(currframe == &frame1) { currframe=&frame2; SetFrameBuffer(framebuffer1); }
        else { currframe=&frame1; SetFrameBuffer(framebuffer2); }

        ClearBitmap(currframe);

        greets_inner(currframe);

//        volatile int delay;
//        for(delay=0; delay<500000; ++delay)
            ;
        

        print_vga_line(currframe);

	}

	while(UserButtonState() && (!done));
}

void RevisionLogo(){
	uint8_t *framebuffer1=(uint8_t *)0x20000000;
	uint8_t *framebuffer2=(uint8_t *)0x20010000;
	memset(framebuffer1,0,320*200);
	memset(framebuffer2,0,320*200);

	SetVGAScreenMode320x200(framebuffer1);


    glyph_t glyph = font_enri_glyph[50];

	Bitmap frame1,frame2;
	InitializeBitmap(&frame1,320,200,320,framebuffer1);
	InitializeBitmap(&frame2,320,200,320,framebuffer2);
	Bitmap *currframe = &frame1;
    
    logo_init();

	while(!UserButtonState() && (!done))
	{
		WaitVBL();

        if(currframe == &frame1) { currframe=&frame2; SetFrameBuffer(framebuffer1); }
        else { currframe=&frame1; SetFrameBuffer(framebuffer2); }

        ClearBitmap(currframe);

        logo_inner(currframe);

	}

	while(UserButtonState() && (!done));
}
#endif //TESTING

void logo_inner(Bitmap* currframe){
    tick++;
    point_t p={100,70};
    if(tick<64){
        render_text_partial(currframe, " ", p, 100, revision_logo_glyph, tick*2);
    } else if(tick < 128){
        render_text(currframe, " ", p, 100, revision_logo_glyph);
    } else if(tick < 196){
        render_text_partial(currframe, " ", p, 100, revision_logo_glyph, -(tick*2-256));
    } else {
        done = true;
    }
}

void greets_inner(Bitmap* currframe){

    // handle what the hell we do
    tock++;
    if(tock>=1){
        tick++;
        tock=0;
    }
    if(tick>67){
        tick = 0;
        greetindex++;
        gprev = greets + greetindex-1;
        gcur = greets + greetindex;
        gnext = greets + greetindex+1;
        if(!gcur->show && !gnext->show){
            done=true;
            greetindex = 1;
        }
    }
    if(tick<= 34){ // construct cur / finish transition from prev
        // cur
        if(gcur->show){
            render_text_partial(currframe, gcur->text, gcur->pos, gcur->size, font_enri_glyph, tick*128/34);
        }
        // prev
        if(gprev->show && gcur->show){
            int a=200,b=200;
            get_text_points(textpts1, &a, gprev->text, gprev->pos, gprev->size,font_enri_glyph, false);
            get_text_points(textpts2, &b, gcur->text, gcur->pos, gcur->size,font_enri_glyph, true);
            make_transition(currframe, textpts1, a, textpts2, b, (tick*128/34)-128);
        }
    } else { // destruct cur / transition to next
        // cur
        if(gcur->show){
            render_text_partial(currframe, gcur->text, gcur->pos, gcur->size, font_enri_glyph, -(tick-34)*128/33);
        }
        // next
        if(gnext->show && gcur->show){
            int a=200,b=200;
            get_text_points(textpts1, &a, gcur->text, gcur->pos, gcur->size, font_enri_glyph, false);
            get_text_points(textpts2, &b, gnext->text, gnext->pos, gnext->size, font_enri_glyph, true);
            make_transition(currframe, textpts1, a, textpts2, b, (tick-34)*128/33);
        }
    }
}

bool isdone(){
    return(done);
}


void print_vga_line(Bitmap* dest){
    int line=22;
#ifndef TESTING
    line = VGALine;
#endif //TESTING
    point_t p3 = {290,185};
    char s[10];
    s[0] = (line/100)%10 + '0';
    s[1] = (line/10)%10 + '0';
    s[2] = (line)%10 + '0';
    s[3] = 0;
    render_text(dest, s ,p3, 20, font_enri_glyph);
}
