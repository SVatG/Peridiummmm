#include "Greets.h"

#include <string.h>

#include "Random.h"
#include "Global.h"
#include "VGA.h"
#include "Button.h"
#include "Graphics/Bitmap.h"
#include "Graphics/Drawing.h"

#include "ryx/font_enri.h"

char* texts[] = {
	"nuance",
	"mercury",
	"never",
	"k2",
	"Rave Network Overscan",
	"Marvin Minsky",
	"Stroboholics",
	"alpha design",
	"farbrausch",
	"fuckings 2 lamers"
};

void Greets(){
	uint8_t *framebuffer1=(uint8_t *)0x20000000;
	uint8_t *framebuffer2=(uint8_t *)0x20010000;
	memset(framebuffer1,0,320*200);
	memset(framebuffer2,0,320*200);
    static point_t textpts1[200];
    static point_t textpts2[200];

	SetVGAScreenMode320x200(framebuffer1);


    glyph_t glyph = font_enri_glyph[50];

	Bitmap frame1,frame2;
	InitializeBitmap(&frame1,320,200,320,framebuffer1);
	InitializeBitmap(&frame2,320,200,320,framebuffer2);
	Bitmap *currframe = &frame1;

    int textindex=-1;

    greet_t gprev, gcur, gnext;
    gnext.pos.x = RandomInteger()&128;
    gnext.pos.y = RandomInteger()&128;
    gnext.text = texts[textindex];
    gprev.show=false;
    gcur.show=false;
    gnext.show=true;
	int tick=10000;

	while(!UserButtonState())
	{
		WaitVBL();

		if(currframe == &frame2) { currframe=&frame2; SetFrameBuffer(framebuffer1); }
		else { currframe=&frame1; SetFrameBuffer(framebuffer2); }

		ClearBitmap(currframe);

        // handle what the hell we do
        tick++;
        if(tick>67){
            tick = 0;
            textindex++;
            gprev = gcur;
            gcur = gnext;
            gnext.pos.x = RandomInteger()&128;
            gnext.pos.y = RandomInteger()&128;
            if(textindex >= (sizeof(texts)/sizeof(char*))){
                gnext.show = false;
            } else {
                gnext.text = texts[textindex];
            }
        }
        if(tick<= 34){ // construct cur / finish transition from prev
            // cur
            if(gcur.show){
                render_text_partial(currframe, gcur.text, gcur.pos, 70, font_enri_glyph, tick*100/34);
            }
            // prev
            if(gprev.show){
                int a=200,b=200;
                get_text_points(textpts1, &a, gprev.text, gprev.pos, 70,font_enri_glyph, false);
                get_text_points(textpts2, &b, gcur.text, gcur.pos, 70,font_enri_glyph, true);
                make_transition(currframe, textpts1, a, textpts2, b, (tick*100/34)+100);
            }
        } else { // destruct cur / transition to next
            // cur
            if(gcur.show){
                render_text_partial(currframe, gcur.text, gcur.pos, 70, font_enri_glyph, (tick-34)*100/33);
            }
            // next
            if(gnext.show){
                int a=200,b=200;
                get_text_points(textpts1, &a, gcur.text, gcur.pos, 70, font_enri_glyph, false);
                get_text_points(textpts2, &b, gnext.text, gnext.pos, 70, font_enri_glyph, true);
                make_transition(currframe, textpts1, a, textpts2, b, (tick-34)*100/33);
            }
        }
	}

	while(UserButtonState());
}
