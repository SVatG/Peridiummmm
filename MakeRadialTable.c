#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define SAMPLES 4
#define WIDTH 106
#define HEIGHT 66
#define BytesPerRow 320

static uint32_t lookup1[HEIGHT][WIDTH][SAMPLES];
static uint32_t lookup2[HEIGHT][WIDTH][SAMPLES];
static uint32_t lookup3[HEIGHT][WIDTH][SAMPLES];
static uint32_t lookup4[HEIGHT][WIDTH][SAMPLES];

int frame;

int main()
{
	for(int y=0;y<2*HEIGHT;y++)
	for(int x=0;x<2*WIDTH;x++)
	for(int i=0;i<SAMPLES;i++)
	{
		float fx=x-WIDTH+1,fy=y-HEIGHT+1;
		float r=sqrtf(fx*fx+fy*fy);
		float dx=fx/r,dy=fy/r;
		int ix=-(float)(i+1)*dx*256;
		int iy=-(float)(i+1)*dy*256;

		int offs=(iy>>8)*BytesPerRow+(ix>>8);
		int xblend=(ix&0xff)>>1;
		int yblend=(iy&0xff)>>1;

		//uint32_t val=(offs&0xffff)|(xblend<<16)|(yblend<<24);
		uint32_t val=((offs&0xffff)<<16)|(xblend<<8)|(yblend<<0);

		if(y<HEIGHT)
		{
			if(x<WIDTH) lookup1[HEIGHT-1-y][WIDTH-1-x][i]=val;
			else lookup2[HEIGHT-1-y][x-WIDTH][i]=val;
		}
		else
		{
			if(x<WIDTH) lookup3[y-HEIGHT][WIDTH-1-x][i]=val;
			else lookup4[y-HEIGHT][x-WIDTH][i]=val;
		}
	}

	uint32_t *table;

	table=&lookup1[0][0][0];
	printf("const uint32_t lookup1[HEIGHT*WIDTH*SAMPLES]=\n");
	printf("{");
	for(int i=0;i<HEIGHT*WIDTH*SAMPLES;i++)
	{
		if(i%8==0) printf("\n\t");
		printf("0x%08x,",table[i]);
	}
	printf("\n");
	printf("};\n");

	table=&lookup2[0][0][0];
	printf("const uint32_t lookup2[HEIGHT*WIDTH*SAMPLES]=\n");
	printf("{");
	for(int i=0;i<HEIGHT*WIDTH*SAMPLES;i++)
	{
		if(i%8==0) printf("\n\t");
		printf("0x%08x,",table[i]);
	}
	printf("\n");
	printf("};\n");

	table=&lookup3[0][0][0];
	printf("const uint32_t lookup3[HEIGHT*WIDTH*SAMPLES]=\n");
	printf("{");
	for(int i=0;i<HEIGHT*WIDTH*SAMPLES;i++)
	{
		if(i%8==0) printf("\n\t");
		printf("0x%08x,",table[i]);
	}
	printf("\n");
	printf("};\n");

	table=&lookup4[0][0][0];
	printf("const uint32_t lookup4[HEIGHT*WIDTH*SAMPLES]=\n");
	printf("{");
	for(int i=0;i<HEIGHT*WIDTH*SAMPLES;i++)
	{
		if(i%8==0) printf("\n\t");
		printf("0x%08x,",table[i]);
	}
	printf("\n");
	printf("};\n");
}
