#include "SpanScreen.h"
#include "Global.h"
#include "VGA.h"
#include "LED.h"
#include "Button.h"
#include "Utils.h"

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
	// Configure timer 8 as the pixel clock.
	TIM8->CR1=TIM_CR1_OPM; // Upcounting mode, one pulse.
	TIM8->DIER=TIM_DIER_CC1IE|TIM_DIER_UIE; // Enable compare interrupts 1, 2, 3 and 4, and update interrupt.
	TIM8->PSC=0; // Prescaler = 1
	TIM8->ARR=4200;

	InstallInterruptHandler(TIM8_UP_TIM13_IRQn,UpdateInterruptHandler);
	EnableInterrupt(TIM8_UP_TIM13_IRQn);
	SetInterruptPriority(TIM8_UP_TIM13_IRQn,0);

	InstallInterruptHandler(TIM8_CC_IRQn,CompareInterruptHandler);
	EnableInterrupt(TIM8_CC_IRQn);
	SetInterruptPriority(TIM8_CC_IRQn,0);

	SetVGAHorizontalSync31kHz(SpanHSyncHandler);

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

		for(int y=0;y<400;y++)
		{
			StartSpans(spans);
{
			int angle=(isin(t*4-15634)*isin(y*3+isin(t*7+166)-4143)>>12)+isin(t*3-3242);

			int32_t sin_a=isin(angle);
			int32_t cos_a=icos(angle);

			int x1=sin_a/8+cos_a/4+Fix(0.5);
			int x2=sin_a/8-cos_a/4+Fix(0.5);
			int x3=-sin_a/8-cos_a/4+Fix(0.5);
			int x4=-sin_a/8+cos_a/4+Fix(0.5);

int c1=(-sin_a+cos_a+Fix(2))>>6;
int c2=(-sin_a-cos_a+Fix(2))>>6;
int c3=(sin_a-cos_a+Fix(2))>>6;
int c4=(sin_a+cos_a+Fix(2))>>6;

			AddSpan(spans,x1,x2,RGB(c1,0,0));
			AddSpan(spans,x2,x3,RGB(0,c2,0));
			AddSpan(spans,x3,x4,RGB(0,0,c3));
			AddSpan(spans,x4,x1,RGB(c4,c4,0));
}
{
			int angle=(isin(t*4+634)*isin(y*3+isin(t*7)+1243)>>12)+isin(t*3-342);

			int32_t sin_a=isin(angle);
			int32_t cos_a=icos(angle);

			int x1=sin_a/16+cos_a/8+Fix(0.25);
			int x2=sin_a/16-cos_a/8+Fix(0.25);
			int x3=-sin_a/16-cos_a/8+Fix(0.25);
			int x4=-sin_a/16+cos_a/8+Fix(0.25);

int c1=(-sin_a+cos_a+Fix(2))>>5;
int c2=(-sin_a-cos_a+Fix(2))>>5;
int c3=(sin_a-cos_a+Fix(2))>>5;
int c4=(sin_a+cos_a+Fix(2))>>5;

			AddSpan(spans,x1,x2,RGB(c1,0,0));
			AddSpan(spans,x2,x3,RGB(0,c2,0));
			AddSpan(spans,x3,x4,RGB(0,0,c3));
			AddSpan(spans,x4,x1,RGB(c4,c4,0));
}
{
			int angle=(isin(t*4+1634)*isin(y*3+isin(t*7+12566)-443)>>12)+isin(t*3+342);

			int32_t sin_a=isin(angle);
			int32_t cos_a=icos(angle);

			int x1=sin_a/16+cos_a/8+Fix(0.75);
			int x2=sin_a/16-cos_a/8+Fix(0.75);
			int x3=-sin_a/16-cos_a/8+Fix(0.75);
			int x4=-sin_a/16+cos_a/8+Fix(0.75);

int c1=(-sin_a+cos_a+Fix(2))>>5;
int c2=(-sin_a-cos_a+Fix(2))>>5;
int c3=(sin_a-cos_a+Fix(2))>>5;
int c4=(sin_a+cos_a+Fix(2))>>5;

			AddSpan(spans,x1,x2,RGB(c1,0,0));
			AddSpan(spans,x2,x3,RGB(0,c2,0));
			AddSpan(spans,x3,x4,RGB(0,0,c3));
			AddSpan(spans,x4,x1,RGB(c4,c4,0));
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
	SetBlankVGAScreenMode200();
	DisableInterrupt(TIM8_UP_TIM13_IRQn);
	DisableInterrupt(TIM8_CC_IRQn);
	TIM8->CR1=0;

	while(UserButtonState());
}

static bool stopped=false;

static void SpanHSyncHandler()
{
	int line=HandleVGAHSync200();
	if(line<0) return;

	TIM8->CR1|=TIM_CR1_CEN; // Start timer;

	SetVGASignal(*data.spanscreen.colourpointer++);

	uint16_t nexttime=*data.spanscreen.timepointer++;

	while(nexttime<TIM8->CNT+100)
	{
		while(TIM8->CNT<nexttime);
		SetVGASignal(*data.spanscreen.colourpointer++);
		nexttime=*data.spanscreen.timepointer++;
	}

	TIM8->CCR1=nexttime-20;

stopped=false;
//	EnableInterrupt(TIM8_CC_IRQn);
}


static void UpdateInterruptHandler()
{
	uint32_t sr=TIM8->SR;
	TIM8->SR=0;

	SetVGASignalToBlack();

stopped=true;
//	DisableInterrupt(TIM8_CC_IRQn);
}

static void CompareInterruptHandler()
{
	uint32_t sr=TIM8->SR;
	TIM8->SR=0;
if(stopped) return;

	SetVGASignal(*data.spanscreen.colourpointer++);

	uint16_t nexttime=*data.spanscreen.timepointer++;

	while(nexttime<TIM8->CNT+100)
	{
		while(TIM8->CNT<nexttime);
		SetVGASignal(*data.spanscreen.colourpointer++);
		nexttime=*data.spanscreen.timepointer++;
	}

	TIM8->CCR1=nexttime-20;
}
