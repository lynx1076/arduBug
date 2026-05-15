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
	src/io.c \
	src/debugger.c \

COMMON_SRCS = \
	../common/ucobs.c

INCLUDES = \
	include \
	../common

CFLAGS = \
	-Wall \
	-Wextra \
	-std=c11 \
	-D_GNU_SOURCE \
	-D_POSIX_C_SOURCE=199309L \
	$(addprefix -I,$(INCLUDES)) \
	$(OPT_FLAG) \
	$(DEBUG_FLAGS)

OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)
OBJS += $(patsubst ../common/%.c,$(BUILD_DIR)/common/%.o,$(COMMON_SRCS))

all: $(TARGET_ELF)
ifeq ($(DEBUG), 1)
	@echo "Compiling with debug symbols"
endif

$(TARGET_ELF): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Rule for local project files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for external common files
$(BUILD_DIR)/common/%.o: ../common/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET_ELF)
	./$(TARGET_ELF)

clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned $(BUILD_DIR) directory"

.PHONY: all clean run

