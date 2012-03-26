#include "Audio.h"
#include "RCC.h"
#include "GPIO.h"
#include "stm32f4xx.h"

static void WriteRegister(uint8_t address,uint8_t value);

void InitializeAudio(int plln,int pllr,int i2sdiv,int i2sodd)
{
	// Turn on peripherals.
	EnableAHB1PeripheralClock(RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOCEN|RCC_AHB1ENR_GPIODEN);
	EnableAPB1PeripheralClock(RCC_APB1ENR_I2C1EN|RCC_APB1ENR_SPI3EN);

	// Configure reset pin.
	SetGPIOOutputMode(GPIOD,1<<4);
	SetGPIOPushPullOutput(GPIOD,1<<4);
	SetGPIOSpeed50MHz(GPIOD,1<<4);
	SetGPIONoPullResistor(GPIOD,1<<4);

	// Configure I2C SCL and SDA pins.
	SetGPIOAlternateFunctionMode(GPIOB,(1<<6)|(1<<9));
	SetGPIOOpenDrainOutput(GPIOB,(1<<6)|(1<<9));
	SetGPIOSpeed50MHz(GPIOB,(1<<6)|(1<<9));
	SetGPIONoPullResistor(GPIOB,(1<<6)|(1<<9));
	SelectAlternateFunctionForGPIOPin(GPIOB,6,4);
	SelectAlternateFunctionForGPIOPin(GPIOB,9,4);

	// Configure I2S MCK, SCK, SD pins.
	SetGPIOAlternateFunctionMode(GPIOC,(1<<7)|(1<<10)|(1<<12));
	SetGPIOPushPullOutput(GPIOC,(1<<7)|(1<<10)|(1<<12));
	SetGPIOSpeed50MHz(GPIOC,(1<<7)|(1<<10)|(1<<12));
	SetGPIONoPullResistor(GPIOC,(1<<7)|(1<<10)|(1<<12));
	SelectAlternateFunctionForGPIOPin(GPIOC,7,6);
	SelectAlternateFunctionForGPIOPin(GPIOC,10,6);
	SelectAlternateFunctionForGPIOPin(GPIOC,12,6);

	// Configure I2S WS pin.
	SetGPIOAlternateFunctionMode(GPIOA,1<<4);
	SetGPIOPushPullOutput(GPIOA,1<<4);
	SetGPIOSpeed50MHz(GPIOA,1<<4);
	SetGPIONoPullResistor(GPIOA,1<<4);
	SelectAlternateFunctionForGPIOPin(GPIOA,4,6);

	// Reset the codec.
	GPIOD->BSRRH=1<<4;
	//Delay(0x4FFF); 
	for(volatile int i=0;i<0x4fff;i++) __asm__ volatile("nop");
	GPIOD->BSRRL=1<<4;

	// Reset I2C.
	SetAPB1PeripheralReset(RCC_APB1RSTR_I2C1RST);
	ClearAPB1PeripheralReset(RCC_APB1RSTR_I2C1RST);

	// Configure I2C.
	uint32_t pclk1=PCLK1Frequency();

	I2C1->CR2=pclk1/1000000; // Configure frequency and disable interrupts and DMA.
	I2C1->OAR1=I2C_OAR1_ADDMODE|0x33;

	// Configure I2C speed in standard mode.
	const uint32_t i2c_speed=100000;
	int ccrspeed=pclk1/(i2c_speed*2);
	if(ccrspeed<4) ccrspeed=4;
	I2C1->CCR=ccrspeed;
	I2C1->TRISE=pclk1/1000000+1;

	I2C1->CR1=I2C_CR1_ACK|I2C_CR1_PE; // Enable and configure the I2C peripheral.

	// Configure codec.
	WriteRegister(0x02,0x01); // Keep codec powered off.
	WriteRegister(0x04,0xaf); // SPK always off and HP always on.

	WriteRegister(0x05,0x81); // Clock configuration: Auto detection.
	WriteRegister(0x06,0x04); // Set slave mode and Philips audio standard.

	SetAudioVolume(0x7f);

	// Power on the codec.
	WriteRegister(0x02,0x9e);

	// Configure codec for fast shutdown.
	WriteRegister(0x0a,0x00); // Disable the analog soft ramp.
	WriteRegister(0x0e,0x04); // Disable the digital soft ramp.

	WriteRegister(0x27,0x00); // Disable the limiter attack level.
	WriteRegister(0x1f,0x0f); // Adjust bass and treble levels.

	WriteRegister(0x1a,0x0a); // Adjust PCM volume level.
	WriteRegister(0x1b,0x0a);

	// Disable I2S.
 	SPI3->I2SCFGR=0;

	// I2S clock configuration
	RCC->CFGR&=~RCC_CFGR_I2SSRC; // PLLI2S clock used as I2S clock source.
	RCC->PLLI2SCFGR=(pllr<<28)|(plln<<6);

	// Enable PLLI2S and wait until it is ready.
 	RCC->CR|=RCC_CR_PLLI2SON;
	while(!(RCC->CR&RCC_CR_PLLI2SRDY));

	// Configure I2S.
	SPI3->I2SPR=i2sdiv|(i2sodd<<8)|SPI_I2SPR_MCKOE;
 	SPI3->I2SCFGR=SPI_I2SCFGR_I2SMOD|SPI_I2SCFGR_I2SCFG_1|SPI_I2SCFGR_I2SE; // Master transmitter, Phillips mode, 16 bit values, clock polarity low, enable.
}

void MakeNoise()
{
	for(;;)
	{
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0x7fff;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0x7fff;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0x7fff;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0x7fff;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0x7fff;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0x7fff;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0x7fff;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0x7fff;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0x7fff;
		while(!(SPI3->SR&SPI_SR_TXE)); SPI3->DR=0x7fff;
	}

}

void AudioOn()
{
}

void AudioOff()
{
}

void SetAudioVolume(int volume)
{
	WriteRegister(0x20,(volume+0x19)&0xff);
	WriteRegister(0x21,(volume+0x19)&0xff);
}

int SetAudioFormat(int samplerate,int bitspersample,int channels)
{
}

void PlayAudioWithCallback(AudioCallbackFunction *callback,void *context)
{
}

void StopAudio()
{
}

void ProvideAudioBuffer(void *samples,int numsamples)
{
}

bool ProvideAudioBufferWithoutBlocking(void *samples,int numsamples)
{
}

static void WriteRegister(uint8_t address,uint8_t value)
{
	while(I2C1->SR2&I2C_SR2_BUSY);

	I2C1->CR1|=I2C_CR1_START; // Start the transfer sequence.

	while(!(I2C1->SR1&I2C_SR1_SB)); // Wait for start bit.

	I2C1->DR=0x94;

	while(!(I2C1->SR1&I2C_SR1_ADDR)); // Wait for master transmitter mode.
	I2C1->SR2;

	I2C1->DR=address; // Transmit the address to write to.

	while(!(I2C1->SR1&I2C_SR1_TXE)); // Wait for byte to move to shift register.

	I2C1->DR=value; // Transmit the value.

	while(!(I2C1->SR1&I2C_SR1_BTF)); // Wait for all bytes to finish.

	I2C1->CR1|=I2C_CR1_STOP; // End the transfer sequence.
}
