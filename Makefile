NAME = VGA

CC = arm-eabi-gcc
LD = arm-eabi-gcc
OBJCOPY = arm-eabi-objcopy

DEFINES =	-DARM_MATH_CM4 \
		-DRGB8Pixels

C_OPTS =	-std=c99 \
		-mthumb \
		-mcpu=cortex-m4 \
		-ILibraries/CMSIS/Include \
		-g \
		-Werror \
		-O3

LIBS =	-lm

SOURCE_DIR = .
BUILD_DIR = Build

C_FILES =	Accelerometer.c \
		Audio.c \
		BitBin.c \
		Button.c \
		Epileptor.c \
		LED.c \
		Main.c \
		OL16Font.c \
		OLFont.c \
		Radial.c \
		Random.c \
		Rotozoomer.c \
		Scroller.c \
		SpanScreen.c \
		Sprites.c \
		Starfield.c \
		Startup.c \
		System.c \
		TestSong.c \
		VGA.c \
		Graphics/Bitmap.c \
		Graphics/MonoFont.c \
		Graphics/ColourFont.c \
		Graphics/DrawingBitmaps.c \
		Graphics/DrawingLines.c \
		Graphics/DrawingPixels.c \
		Graphics/DrawingRectangles.c \
		Graphics/DrawingRLEBitmaps.c \
		Graphics/DrawingStraightLines.c \
		Graphics/Font.c \
		VectorLibrary/Integer.c \
		font.c \
		bezier.c \
		Greets.c \
		font_enri.c \
		revision_logo.c \
		VectorLibrary/VectorFixed.c \
		VectorLibrary/MatrixFixed.c \
		VectorLibrary/QuaternionFixed.c \
		Rasterize.c \
		Metablobs.c \
		Blobs.c \
		logo.c \
		LogoShow.c \
		Scroller2.c \
			
S_FILES = 

OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o) $(S_FILES:%.S=$(BUILD_DIR)/%.o)

ALL_CFLAGS = $(C_OPTS) $(DEFINES) $(CFLAGS)
ALL_LDFLAGS = $(LD_FLAGS) -mthumb -mcpu=cortex-m4 -nostartfiles -Wl,-T,Linker.ld,--gc-sections -specs Terrible.specs

AUTODEPENDENCY_CFLAGS=-MMD -MF$(@:.o=.d) -MT$@





all: $(NAME).bin

upload: $(NAME).bin
	openocd -f interface/stlink-v2.cfg -f target/stm32f4x_stlink.cfg \
	-c init -c "reset halt" -c "stm32f2x mass_erase 0" \
	-c "flash write_bank 0 $(NAME).bin 0" \
	-c "reset run" -c shutdown


debug:
	arm-eabi-gdb $(NAME).elf \
	--eval-command="target remote | openocd -f interface/stlink-v2.cfg -f target/stm32f4x_stlink.cfg -c 'gdb_port pipe'"

stlink:
	arm-eabi-gdb $(NAME).elf --eval-command="target ext :4242"

clean:
	rm -rf $(BUILD_DIR) $(NAME).elf $(NAME).bin

$(NAME).bin: $(NAME).elf
	$(OBJCOPY) -O binary $(NAME).elf $(NAME).bin

$(NAME).elf: $(OBJS)
	$(LD) $(ALL_LDFLAGS) -o $@ $^ $(LIBS)

#font_enri.c: fontToBezier.py
#	python fontToBezier.py

.SUFFIXES: .o .c .S

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(ALL_CFLAGS) $(AUTODEPENDENCY_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(ALL_CFLAGS) $(AUTODEPENDENCY_CFLAGS) -c $< -o $@

-include $(OBJS:.o=.d)

