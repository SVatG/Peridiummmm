#include "SpanScreen.h"
#include "Global.h"
#include "VGA.h"
#include "LED.h"
#include "Button.h"
#include "Utils.h"
#include "RCC.h"

#include "Graphics/Pixels.h"

#include <stdint.h>
#include <string.h>

static void SpanHSyncHandler();
static void UpdateInterruptHandler();
static void CompareInterruptHandler();

typedef struct Span
{
	uint16_t end;
	uint8_t colour;
} Span;

static inline void StartSpans(Span *spans)
{
	spans->end=0xffff;
	spans->colour=0;
}

static inline void AddSpan(Span *spans,int32_t x1,int32_t x2,uint8_t col)
{
	if(x1>x2) return;
	if(x2<0) return;
	if(x1<0)
	{
		x1=0;
	}

	Span *head=spans;
	while(head->end<x1) head++;

	Span *tail=head;
	while(tail->end<=x2) tail++;

	if(tail!=head+2)
	{
		Span *end=tail;
		while(end->end!=0xffff) end++;
		memmove(head+2,tail,(end-tail+1)*sizeof(Span));
	}

	head[0].end=x1;
	head[1].end=x2;
	head[1].colour=col;
}


void SpanScreen()
{
	EnableAPB2PeripheralClock(RCC_APB2ENR_TIM1EN);

	uint8_t palettes[3][256];
	for(int i=0;i<256;i++)
	{
		palettes[0][i]=RGB(i*2,i*2,i*9/4);
		palettes[1][i]=RGB(i*3,i*3-256,i*2);
		palettes[2][i]=RGB(i*3-256,i*2,i*3);
	}

	uint16_t *timepointer=data.spanscreen.times[0];
	uint8_t *colourpointer=data.spanscreen.colours[0];
	for(int y=0;y<400;y++)
	{
		*colourpointer++=RGB(0,0,0);
		*timepointer++=0xffff;
	}
	data.spanscreen.timepointer=data.spanscreen.times[0];
	data.spanscreen.colourpointer=data.spanscreen.colours[0];

	WaitVBL();
	SetVGAHorizontalSync31kHz(SpanHSyncHandler);

	// Configure timer 1 as the pixel clock.
	TIM1->CR1=TIM_CR1_OPM; // Upcounting mode, one pulse.
	TIM1->DIER=TIM_DIER_CC1IE|TIM_DIER_UIE; // Enable compare interrupts 1, 2, 3 and 4, and update interrupt.
	TIM1->PSC=0; // Prescaler = 1
	TIM1->ARR=4100;
	TIM1->SR=0;

	InstallInterruptHandler(TIM1_UP_TIM10_IRQn,UpdateInterruptHandler);
	SetInterruptPriority(TIM1_UP_TIM10_IRQn,0);
	EnableInterrupt(TIM1_UP_TIM10_IRQn);

	InstallInterruptHandler(TIM1_CC_IRQn,CompareInterruptHandler);
	SetInterruptPriority(TIM1_CC_IRQn,0);
	EnableInterrupt(TIM1_CC_IRQn);

	int frame=0;

	Span spans[32];

	while(!UserButtonState())
	{
		WaitVBL();

		data.spanscreen.timepointer=data.spanscreen.times[frame];
		data.spanscreen.colourpointer=data.spanscreen.colours[frame];

		int t=VGAFrameCounter();
		SetLEDs(1<<((t/3)&3));

		uint16_t *timepointer=data.spanscreen.times[frame^1];
		uint8_t *colourpointer=data.spanscreen.colours[frame^1];

		int mainangle=(t*15)&4095;
		int order[3];
		int depth0=icos(mainangle+0*((4096+2)/3));
		int depth1=icos(mainangle+1*((4096+2)/3));
		int depth2=icos(mainangle+2*((4096+2)/3));
		if(depth0<depth1)
		{
			if(depth1<depth2) { order[0]=0; order[1]=1; order[2]=2; } // 0 < 1 < 2
			else if(depth0<depth2) { order[0]=0; order[1]=2; order[2]=1; } // 0 < 2 < 1
			else { order[0]=2; order[1]=0; order[2]=1; } // 2 < 0 < 1
		}
		else
		{
			if(depth1>depth2) { order[0]=2; order[1]=1; order[2]=0; } // 2 < 1 < 0
			else if(depth0>depth2) { order[0]=1; order[1]=2; order[2]=0; } // 1 < 2 < 0
			else { order[0]=1; order[1]=0; order[2]=2; } // 1 < 0 < 2
		}

		for(int y=0;y<400;y++)
		{
			StartSpans(spans);

			for(int i=0;i<3;i++)
			{
				int n=order[i];
				int angle=(isin(t*4-634*n)*isin(y*3+t*16-1876*n)>>12)+isin(t*3-3242*n);

				int32_t sin_a=isin(angle);
				int32_t cos_a=icos(angle);

				int center=isin(mainangle+n*((4096+2)/3))/3+2050;
				int depth=icos(mainangle+n*((4096+2)/3))+Fix(2);

				int x1=sin_a/16+cos_a/8+center;
				int x2=sin_a/16-cos_a/8+center;
				int x3=-sin_a/16-cos_a/8+center;
				int x4=-sin_a/16+cos_a/8+center;
				int c1=(depth*(-sin_a+cos_a+Fix(2)))>>20;
				int c2=(depth*(-sin_a-cos_a+Fix(2)))>>20;
				int c3=(depth*(sin_a-cos_a+Fix(2)))>>20;
				int c4=(depth*(sin_a+cos_a+Fix(2)))>>20;
				if(c1>256) c1=255;
				if(c2>256) c2=255;
				if(c3>256) c3=255;
				if(c4>256) c4=255;

				AddSpan(spans,x1,x2,palettes[n][c1]);
				AddSpan(spans,x2,x3,palettes[n][c2]);
				AddSpan(spans,x3,x4,palettes[n][c3]);
				AddSpan(spans,x4,x1,palettes[n][c4]);
			}

			for(Span *span=spans;span->end!=0xffff;span++)
			{
				*colourpointer++=span->colour;
				*timepointer++=span->end;
			}
			*colourpointer++=RGB(0,0,0);
			*timepointer++=0xffff;
		}

		frame^=1;
	}

	WaitVBL();
	SetBlankVGAScreenMode400();
	DisableInterrupt(TIM1_UP_TIM10_IRQn);
	DisableInterrupt(TIM1_CC_IRQn);
	TIM1->CR1=0;

	while(UserButtonState());
}

static bool stopped=false;

static void SpanHSyncHandler()
{
	int line=HandleVGAHSync400();
	if(line<0) return;

	TIM1->CR1|=TIM_CR1_CEN; // Start timer;

	uint8_t *colptr=data.spanscreen.colourpointer;
	uint16_t *timeptr=data.spanscreen.timepointer;

	SetVGASignal(*colptr++);

	uint16_t nexttime=*timeptr++;

	while(nexttime<TIM1->CNT+100)
	{
		while(TIM1->CNT<nexttime);
		SetVGASignal(*colptr++);
		nexttime=*timeptr++;
	}

	data.spanscreen.colourpointer=colptr;
	data.spanscreen.timepointer=timeptr;

	TIM1->CCR1=nexttime-30;

stopped=false;
//	EnableInterrupt(TIM1_CC_IRQn);
}


static void UpdateInterruptHandler()
{
	uint32_t sr=TIM1->SR;
	TIM1->SR=0;

	SetVGASignalToBlack();

stopped=true;
//	DisableInterrupt(TIM1_CC_IRQn);
}

static void CompareInterruptHandler()
{
	uint32_t sr=TIM1->SR;
	TIM1->SR=0;
if(stopped) return;

	uint8_t *colptr=data.spanscreen.colourpointer;
	uint16_t *timeptr=data.spanscreen.timepointer;

	SetVGASignal(*colptr++);

	uint16_t nexttime=*timeptr++;

	while(nexttime<TIM1->CNT+100)
	{
		while(TIM1->CNT<nexttime);
		SetVGASignal(*colptr++);
		nexttime=*timeptr++;
	}

	data.spanscreen.colourpointer=colptr;
	data.spanscreen.timepointer=timeptr;

	TIM1->CCR1=nexttime-30;
}
