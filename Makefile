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

#			Utilities/STM32F4-Discovery/stm32f4_discovery.c \
#			Utilities/STM32F4-Discovery/stm32f4_discovery_lis302dl.c \
#			Utilities/STM32F4-Discovery/stm32f4_discovery_audio_codec.c

C_FILES =	Accelerometer.c \
			Button.c \
			LED.c \
			Main.c \
			Random.c \
			Sprites.c \
			Startup.c \
			System.c \
			Utils.c \
			VGA.c \
			Graphics/Bitmap.c \
			Graphics/DrawingBitmaps.c \
			Graphics/DrawingLines.c \
			Graphics/DrawingPixels.c \
			Graphics/DrawingRectangles.c \
			Graphics/DrawingRLEBitmaps.c \
			Graphics/DrawingStraightLines.c
			
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

clean:
	rm -rf $(BUILD_DIR) $(NAME).elf $(NAME).bin

$(NAME).bin: $(NAME).elf
	$(OBJCOPY) -O binary $(NAME).elf $(NAME).bin

$(NAME).elf: $(OBJS)
	$(LD) $(ALL_LDFLAGS) -o $@ $^ $(LIBS)

.SUFFIXES: .o .c .S

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(ALL_CFLAGS) $(AUTODEPENDENCY_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(ALL_CFLAGS) $(AUTODEPENDENCY_CFLAGS) -c $< -o $@

-include $(OBJS:.o=.d)

