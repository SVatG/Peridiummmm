#include <stdint.h>

#include "System.h"
#include "LED.h"
#include "Button.h"
#include "VGA.h"
#include "RCC.h"
#include "Audio.h"
#include "Random.h"
#include "Utils.h"
#include "BitBin.h"
#include "Global.h"

#include "Metablobs.h"
#include "Rotozoomer.h"
#include "Starfield.h"
#include "Rasterize.h"
#include "Epileptor.h"
#include "Scroller.h"
#include "ryx/font.h"
#include "ryx/font_enri.h"

static void Fonttest();

static void AudioCallback(void *context,int buffer);
int16_t *buffers[2]={ (int16_t *)0x2001fa00,(int16_t *)0x2001fc00 };
extern BitBinNote *channels[8];

union GlobalData data;

int main()
{
	InitializeLEDs();

	InitializeSystem();

	InitializeLEDs();
	InitializeUserButton();

	BitBinSong song;
	InitializeBitBinSong(&song,BitBin22kTable,8,channels);

	InitializeAudio(Audio22050HzSettings);
	SetAudioVolume(0xaf);
	PlayAudioWithCallback(AudioCallback,&song);

	InitializeVGAScreenMode200();

	for(;;)
	{
		SpanScreen();
        Fonttest();
		Epileptor();
		Starfield();
		Metablobs();
		Scroller("Look out honey coz I'm using technology, ain't got time to make no apology");
		Rasterize();
		Rotozoom();
	}
}

static void AudioCallback(void *context,int buffer)
{
	BitBinSong *song=context;

	int16_t *samples=buffers[buffer];
	RenderBitBinSamples(song,128,samples);
	for(int i=128;i>=0;i--)
	{
		samples[2*i+0]=samples[i];
		samples[2*i+1]=samples[i];
	}

	ProvideAudioBuffer(samples,256);
}


static void Fonttest(){
	uint8_t *framebuffer1=(uint8_t *)0x20000000;
	uint8_t *framebuffer2=(uint8_t *)0x20010000;
	memset(framebuffer1,0,320*200);
	memset(framebuffer2,0,320*200);
    static point_t textpts1[200];
    static point_t textpts2[200];

	SetVGAScreenMode320x200(framebuffer1);

	InitializeLEDFlow();

    glyph_t glyph = font_enri_glyph[50];

	Bitmap frame1,frame2;
	InitializeBitmap(&frame1,320,200,320,framebuffer1);
	InitializeBitmap(&frame2,320,200,320,framebuffer2);

	int frame=0;

	while(!UserButtonState())
	{
		WaitVBL();

		RunLEDFlow();

		Bitmap *currframe;
		if(frame&1) { currframe=&frame2; SetFrameBuffer(framebuffer1); }
		else { currframe=&frame1; SetFrameBuffer(framebuffer2); }

		ClearBitmap(currframe);

        point_t p={40,10};
        point_t p2={40,80};
        if(frame<200){
            render_text_partial(currframe, "mercury",p, 70, font_enri_glyph, frame);
        }
        if(frame>=100 && frame<300){
            int a = 100;
            int b = 100;
            get_text_points(textpts1, &a, "mercury",p, 70,font_enri_glyph, false);
            get_text_points(textpts2, &b, "nuance",p2, 70,font_enri_glyph, true);
            make_transition(currframe, textpts1, a, textpts2, b, frame-100);
            if(frame==105){
    //            printf("%i, %i\n",a,b);
            }
        }
        if(frame>=200 && frame<400){
            render_text_partial(currframe, "nuance",p2, 70, font_enri_glyph, frame-200);
        }
        if(frame>=400){
            frame = 0;
        }

        point_t p3 = {0,170};
        char s[10];
        s[0] = (frame/100)%10 + '0';
        s[1] = (frame/10)%10 + '0';
        s[2] = (frame)%10 + '0';
        render_text_partial(currframe, s ,p, 70, font_enri_glyph, frame);




		frame++;
	}

	while(UserButtonState());
}