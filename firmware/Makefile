TARGET = main

MCU = atmega328p
CC = avr-gcc
OBJCOPY = avr-objcopy

SRCS = \
	src/main.c \
	src/twi.c \
	src/mcp23017.c \
	src/serial.c \
	src/serial_handler.c \
	src/serial_commands.c \

BUILD_DIR = .build

INCLUDES = \
	-Iinclude \
	-I../protocol/

CFLAGS = \
	-Wall \
	-Wextra \
	-Werror \
	-std=c11 \
	-mmcu=$(MCU) \
	-DF_CPU=16000000UL \
	-O3 \
	$(INCLUDES)

LDFLAGS = \
	-mmcu=$(MCU)

OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)

ELF = $(BUILD_DIR)/$(TARGET).elf
HEX = $(BUILD_DIR)/$(TARGET).hex

PORT ?= $(shell ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null | head -n 1)

all: $(HEX)

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(ELF): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

flash: $(HEX)
ifndef PORT
	$(error No serial device found. Plug one in or specify one explicitly: PORT=/dev/ttyUSB0)
endif
	avrdude -c arduino -p m328p -b 115200 -P $(PORT) -U flash:w:$(HEX):a

clean:
	rm -rf $(BUILD_DIR) $(HEX)

.PHONY: all clean flash

