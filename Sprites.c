#include "Graphics/RLEBitmap.h"

const struct
{
	RLEBitmap rle;
	Pixel rows[112];
} Star1=
{
	{ 32,32 },
	{
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,
		0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xe3,
		0xc0,0xe0,0xc0,0xf0,0xd5,0xc0,0xe9,0xfa,
		0xe9,0xc0,0xe0,0xd5,0xe0,0xfa,0xff,0xfa,
		0xe0,0xe0,0xd5,0xc0,0xe9,0xfa,0xe9,0xc0,
		0xe0,0xe3,0xc0,0xe0,0xc0,0xf0,0xf0,0xf0,
		0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,
		0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,
		0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,
	}
};
const struct
{
	RLEBitmap rle;
	Pixel rows[134];
} Star2=
{
	{ 32,32 },
	{
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,
		0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xf0,0xf0,0x20,0xd5,0x40,0xc0,0xc0,
		0xc0,0x40,0xe0,0xc7,0x40,0xe0,0xe9,0xe9,
		0xe9,0xe0,0x40,0xd0,0xc7,0xc0,0xe9,0xfa,
		0xff,0xfa,0xe9,0xc0,0xd0,0xc7,0xc0,0xe9,
		0xff,0xff,0xff,0xe9,0xc0,0xd0,0xc7,0xc0,
		0xe9,0xfa,0xff,0xfa,0xe9,0xc0,0xd0,0xc7,
		0x40,0xe0,0xe9,0xe9,0xe9,0xe0,0x40,0xd0,
		0xd5,0x40,0xc0,0xc0,0xc0,0x40,0xe0,0xf0,
		0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,
		0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,
	}
};
const struct
{
	RLEBitmap rle;
	Pixel rows[144];
} Star3=
{
	{ 32,32 },
	{
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,
		0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xe3,0x40,0x40,0x40,0xf0,0xd5,0xc0,
		0xe0,0xe9,0xe0,0xc0,0xe0,0xc7,0xc0,0xe9,
		0xfa,0xfa,0xfa,0xe9,0xc0,0xd0,0xb9,0x40,
		0xe0,0xfa,0xff,0xff,0xff,0xfa,0xe0,0x40,
		0xc0,0xb9,0x40,0xe9,0xfa,0xff,0xff,0xff,
		0xfa,0xe9,0x40,0xc0,0xb9,0x40,0xe0,0xfa,
		0xff,0xff,0xff,0xfa,0xe0,0x40,0xc0,0xc7,
		0xc0,0xe9,0xfa,0xfa,0xfa,0xe9,0xc0,0xd0,
		0xd5,0xc0,0xe0,0xe9,0xe0,0xc0,0xe0,0xe3,
		0x40,0x40,0x40,0xf0,0xf0,0xf0,0x20,0xf0,
		0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,
		0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,
	}
};
const struct
{
	RLEBitmap rle;
	Pixel rows[152];
} Star4=
{
	{ 32,32 },
	{
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,
		0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf1,0x40,
		0xf0,0x10,0xe3,0x40,0xe0,0x40,0xf0,0xc7,
		0xc0,0x40,0xc0,0xfa,0xc0,0x40,0xc0,0xd0,
		0xc7,0x40,0xe9,0xfa,0xff,0xfa,0xe9,0x40,
		0xd0,0xb9,0x40,0xc0,0xfa,0xff,0xff,0xff,
		0xfa,0xc0,0x40,0xc0,0xab,0x40,0xe0,0xfa,
		0xff,0xff,0xff,0xff,0xff,0xfa,0xe0,0x40,
		0xb0,0xb9,0x40,0xc0,0xfa,0xff,0xff,0xff,
		0xfa,0xc0,0x40,0xc0,0xc7,0x40,0xe9,0xfa,
		0xff,0xfa,0xe9,0x40,0xd0,0xc7,0xc0,0x40,
		0xc0,0xfa,0xc0,0x40,0xc0,0xd0,0xe3,0x40,
		0xe0,0x40,0xf0,0xf1,0x40,0xf0,0x10,0xf0,
		0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,
		0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,
	}
};
const struct
{
	RLEBitmap rle;
	Pixel rows[187];
} Star5=
{
	{ 32,32 },
	{
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,
		0xf0,0x20,0xf0,0xf0,0x20,0xf1,0x40,0xf0,
		0x10,0xf1,0xc0,0xf0,0x10,0xf1,0xe0,0xf0,
		0x10,0xe3,0x40,0xe9,0x40,0xf0,0xb2,0xe0,
		0x40,0x13,0xc0,0xfa,0xc0,0x12,0x40,0xe0,
		0xc0,0xb9,0x40,0xe9,0xc0,0xe0,0xff,0xe0,
		0xc0,0xe9,0x40,0xc0,0xc7,0xc0,0xfa,0xfa,
		0xff,0xfa,0xfa,0xc0,0xd0,0xab,0x40,0xc0,
		0xe0,0xfa,0xff,0xff,0xff,0xfa,0xe0,0xc0,
		0x40,0xb0,0x7f,0x40,0xc0,0xe0,0xe9,0xfa,
		0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfa,
		0xe9,0xe0,0x02,0xc0,0x40,0x80,0xab,0x40,
		0xc0,0xe0,0xfa,0xff,0xff,0xff,0xfa,0xe0,
		0xc0,0x40,0xb0,0xc7,0xc0,0xfa,0xfa,0xff,
		0xfa,0xfa,0xc0,0xd0,0xb9,0x40,0xe9,0xc0,
		0xe0,0xff,0xe0,0xc0,0xe9,0x40,0xc0,0xb2,
		0xe0,0x40,0x13,0xc0,0xfa,0xc0,0x12,0x40,
		0xe0,0xc0,0xe3,0x40,0xe9,0x40,0xf0,0xf1,
		0xe0,0xf0,0x10,0xf1,0xc0,0xf0,0x10,0xf1,
		0x40,0xf0,0x10,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,
		0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,
		0xf0,0xf0,0x20,
	}
};
const struct
{
	RLEBitmap rle;
	Pixel rows[243];
} Star6=
{
	{ 32,32 },
	{
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf1,
		0x40,0xf0,0x10,0xf1,0xc0,0xf0,0x10,0xf1,
		0xe0,0xf0,0x10,0xe3,0x40,0xe9,0x40,0xf0,
		0x91,0x40,0x43,0xc0,0xfa,0xc0,0x41,0x40,
		0xa0,0xa2,0xc0,0x40,0x15,0x40,0xe0,0xfa,
		0xe0,0x40,0x12,0x40,0xc0,0xb0,0xab,0x40,
		0xe0,0xc0,0xc0,0xe9,0xff,0xe9,0xc0,0xc0,
		0xe0,0x40,0xb0,0xb9,0xc0,0xfa,0xe9,0xfa,
		0xff,0xfa,0xe9,0xfa,0xc0,0xc0,0xab,0x40,
		0xc0,0xe9,0xff,0xff,0xff,0xff,0xff,0xe9,
		0xc0,0x40,0xb0,0x8f,0x40,0xc0,0xe0,0xe9,
		0xfa,0xff,0xff,0xff,0xff,0xff,0xfa,0xe9,
		0xe0,0xc0,0x40,0x90,0x5f,0x40,0xc0,0xe0,
		0xe9,0xfa,0xfa,0xff,0xff,0xff,0xff,0xff,
		0xff,0xff,0xff,0xff,0x06,0xfa,0xfa,0xe9,
		0xe0,0xc0,0x40,0x60,0x8f,0x40,0xc0,0xe0,
		0xe9,0xfa,0xff,0xff,0xff,0xff,0xff,0xfa,
		0xe9,0xe0,0xc0,0x40,0x90,0xab,0x40,0xc0,
		0xe9,0xff,0xff,0xff,0xff,0xff,0xe9,0xc0,
		0x40,0xb0,0xb9,0xc0,0xfa,0xe9,0xfa,0xff,
		0xfa,0xe9,0xfa,0xc0,0xc0,0xab,0x40,0xe0,
		0xc0,0xc0,0xe9,0xff,0xe9,0xc0,0xc0,0xe0,
		0x40,0xb0,0xa2,0xc0,0x40,0x15,0x40,0xe0,
		0xfa,0xe0,0x40,0x12,0x40,0xc0,0xb0,0x91,
		0x40,0x43,0xc0,0xfa,0xc0,0x41,0x40,0xa0,
		0xe3,0x40,0xe9,0x40,0xf0,0xf1,0xe0,0xf0,
		0x10,0xf1,0xc0,0xf0,0x10,0xf1,0x40,0xf0,
		0x10,0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,
		0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,
		0xf0,0xf0,0x20,
	}
};
const struct
{
	RLEBitmap rle;
	Pixel rows[287];
} Star7=
{
	{ 32,32 },
	{
		0xf0,0xf0,0x20,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xf1,0x40,0xf0,0x10,0xf1,0xc0,0xf0,
		0x10,0xf1,0xe0,0xf0,0x10,0xf1,0xe9,0xf0,
		0x10,0xe3,0x40,0xfa,0x40,0xf0,0x81,0x40,
		0x53,0xc0,0xfa,0xc0,0x51,0x40,0x90,0x92,
		0xe0,0xc0,0x25,0x40,0xe0,0xff,0xe0,0x40,
		0x22,0xc0,0xe0,0xa0,0x9d,0xc0,0xe9,0xe0,
		0x40,0xc0,0xe9,0xff,0xe9,0xc0,0x40,0xe0,
		0xe9,0xc0,0xa0,0xab,0xe0,0xfa,0xe9,0xe0,
		0xfa,0xff,0xfa,0xe0,0xe9,0xfa,0xe0,0xb0,
		0xab,0x40,0xe9,0xff,0xfa,0xff,0xff,0xff,
		0xfa,0xff,0xe9,0x40,0xb0,0x9d,0x40,0xc0,
		0xe0,0xfa,0xff,0xff,0xff,0xff,0xff,0xfa,
		0xe0,0xc0,0x40,0xa0,0x7f,0x40,0xc0,0xe0,
		0xe9,0xfa,0xff,0xff,0xff,0xff,0xff,0xff,
		0xff,0xfa,0xe9,0xe0,0x02,0xc0,0x40,0x80,
		0x3f,0x40,0xc0,0xe0,0xe9,0xfa,0xfa,0xff,
		0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		0x0a,0xff,0xff,0xff,0xff,0xfa,0xfa,0xe9,
		0xe0,0xc0,0x40,0x40,0x7f,0x40,0xc0,0xe0,
		0xe9,0xfa,0xff,0xff,0xff,0xff,0xff,0xff,
		0xff,0xfa,0xe9,0xe0,0x02,0xc0,0x40,0x80,
		0x9d,0x40,0xc0,0xe0,0xfa,0xff,0xff,0xff,
		0xff,0xff,0xfa,0xe0,0xc0,0x40,0xa0,0xab,
		0x40,0xe9,0xff,0xfa,0xff,0xff,0xff,0xfa,
		0xff,0xe9,0x40,0xb0,0xab,0xe0,0xfa,0xe9,
		0xe0,0xfa,0xff,0xfa,0xe0,0xe9,0xfa,0xe0,
		0xb0,0x9d,0xc0,0xe9,0xe0,0x40,0xc0,0xe9,
		0xff,0xe9,0xc0,0x40,0xe0,0xe9,0xc0,0xa0,
		0x92,0xe0,0xc0,0x25,0x40,0xe0,0xff,0xe0,
		0x40,0x22,0xc0,0xe0,0xa0,0x81,0x40,0x53,
		0xc0,0xfa,0xc0,0x51,0x40,0x90,0xe3,0x40,
		0xfa,0x40,0xf0,0xf1,0xe9,0xf0,0x10,0xf1,
		0xe0,0xf0,0x10,0xf1,0xc0,0xf0,0x10,0xf1,
		0x40,0xf0,0x10,0xf0,0xf0,0x20,0xf0,0xf0,
		0x20,0xf0,0xf0,0x20,0xf0,0xf0,0x20,
	}
};