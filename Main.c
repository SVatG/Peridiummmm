#include <stdint.h>
#include <string.h>

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
#include "LogoShow.h"
#include "Greets.h"
#include "Scroller2.h"
#include "Scroller3.h"

static void AudioCallback(void *context,int buffer);
int16_t *buffers[2]={ (int16_t *)0x2001fa00,(int16_t *)0x2001fc00 };
extern BitBinNote *channels[8];

union GlobalData data;

BitBinSong song;

int main()
{
	InitializeLEDs();

	InitializeSystem();
	
	InitializeLEDs();
	InitializeUserButton();

	InitializeBitBinSong(&song,BitBin22kTable,8,1920,channels);

	InitializeAudio(Audio22050HzSettings);
	SetAudioVolume(0xEF);
	InitializeVGAScreenMode400();

	// "Monitor turns on"
	for(int i = 0; i < 150; i++) {
		WaitVBL();
		// TODO set a black framebuffer
	}
	
	PlayAudioWithCallback(AudioCallback,&song);

/*
1:  BITMAP BLEND IN                                                                                                            00:00 -> 00:04
2:  REVISION LOGO BEZIER BUILD                                                                                                 00:04 -> 00:08
3:  SCROLLER: "Welcome to our little prod for the STM32F4. Lets start things off with some twisters."                          00:08 -> 00:15
4:  TWISTERS                                                                                                                   00:15 -> 00:23
5:  SCROLLER: "4000 px horizontal resulution, can your HD screen handle it? Next is our beautiful rotozoomer."                 00:23 -> 00:30
6:  ROTOZOOMER                                                                                                                 00:30 -> 00:38
7:  SCROLLER: "Who doesn't love those? 640x480 at 70hz. 192 kb ram 1 mb flash infinite possibilities. Time for greets:"        00:38 -> 00:45
8:  BEZIERGREETS                                                                                                               00:45 -> 00:61
9:  STARFIELD                                                                                                                  00:61 -> 00:69
10: SCROLLER: "2000 sprites all software no sweat. Lets break it down with some real 3D in here."                              00:69 -> 00:77
11: RASTERIZER                                                                                                                 00:77 -> 00:88
12: SCROLLER: "SVatG hopes this demo will inspire you to write your own demos on the STM32F4. Until next party, peace out."    00:88 -> 96:00
13: METABLOBS W/ FADE OF SOME KIND                                                                                             00:96 -> END
*/

	
	for(;;)
	{		
		LogoShow();
		RevisionLogo();
		Scroller3("Welcome to our little prod for the STM32F4. Lets start things off with some lovely full-screen twisters.");
		SpanScreen();
		Scroller("4000 pixels of horizontal resulution, can your \"high definition\" screen handle it? Next is our cute and pop rotozoomer.");
		Rotozoom();
		Scroller2("Who doesn't love those? 640x480 at 70hz. 192 kb ram 1 mb flash infinite possibilities. Time for greets:");
		Greets();	
		Starfield();
		Epileptor("2000 sprites all software no sweat. Lets break it down with some real 3D in here.");
		Rasterize();
		RadialScroller("SVatG hopes this demo will inspire you to write your own demos on the STM32F4. Until next party, peace out.");
		Metablobs();
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

