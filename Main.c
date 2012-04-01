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
