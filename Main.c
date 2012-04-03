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

	for(;;)
	{
		RadialScroller("I got stash upon stash, and it ain't no' but cash.");
		Scroller2();
		RevisionLogo();
		Rasterize();
		LogoShow();
		Epileptor("Look out honey coz I'm using technology");
		Greets();
		SpanScreen();
		Starfield();
		Metablobs();
		Scroller("Look out honey coz I'm using technology, ain't got time to make no apology");
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

