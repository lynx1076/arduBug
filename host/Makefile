BUILD_DIR = .build

TARGET = arduBug-cli
TARGET_ELF = $(BUILD_DIR)/$(TARGET)

DEBUG ?= 0
DEBUG_FLAGS =
OPT_FLAG = -O3

ifeq ($(DEBUG), 1)
    DEBUG_FLAGS = -g
    OPT_FLAG = -O0
endif

CC = gcc

SRCS = \
    src/main.c \
    src/vector.c \
    src/serial.c \
    src/utils.c \
    src/result.c \
		src/cli_handler.c \
		src/commands.c \
    ../common/ucobs.c

INCLUDES = \
    include \
    ../common

CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE -D_POSIX_C_SOURCE=199309L $(addprefix -I,$(INCLUDES)) $(OPT_FLAG) $(DEBUG_FLAGS)

OBJS = $(SRCS:%.c=$(BUILD_DIR)/linux/%.o)

all: $(TARGET_ELF)
ifeq ($(DEBUG), 1)
	@echo "Compiling with debug symbols"
endif

$(TARGET_ELF): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/linux/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET_ELF)
	./$(TARGET_ELF)

clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned $(BUILD_DIR) directory"

.PHONY: all clean run

