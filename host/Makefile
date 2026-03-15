BUILD_DIR = .build

TARGET = arduBug

LINUX_TARGET = $(BUILD_DIR)/linux/$(TARGET)
WINDOWS_TARGET = $(BUILD_DIR)/windows/$(TARGET).exe

TARGET_OS ?= linux

LINUX_CC = gcc
WINDOWS_CC = x86_64-w64-mingw32-gcc

SRCS = \
	src/main.c \
	src/vector.c \
	src/serial.c \

INCLUDES = \
	include \
	../protocol \

LINUX_CFLAGS = -Wall -Wextra -std=c11 -g $(addprefix -I,$(INCLUDES))
WINDOWS_CFLAGS = -Wall -Wextra -std=c11 -g $(addprefix -I,$(INCLUDES))

LINUX_LDFLAGS = \
	-lm \
	-pthread \

WINDOWS_LDFLAGS = \
	-pthread

LINUX_OBJS = \
	$(SRCS:%.c=$(BUILD_DIR)/linux/%.o) \

WINDOWS_OBJS = \
	$(SRCS:%.c=$(BUILD_DIR)/windows/%.o) \

all: $(TARGET_OS)

linux: $(LINUX_TARGET)

windows: $(WINDOWS_TARGET)

$(LINUX_TARGET): $(LINUX_OBJS)
	$(LINUX_CC) $(LINUX_OBJS) -o $@ $(LINUX_LDFLAGS)

$(WINDOWS_TARGET): $(WINDOWS_OBJS)
	$(WINDOWS_CC) $(WINDOWS_OBJS) -o $@ $(WINDOWS_LDFLAGS)

$(BUILD_DIR)/linux/%.o: %.c
	@mkdir -p $(dir $@)
	$(LINUX_CC) $(LINUX_CFLAGS) -c $< -o $@

$(BUILD_DIR)/windows/%.o: %.c
	@mkdir -p $(dir $@)
	$(WINDOWS_CC) $(WINDOWS_CFLAGS) -c $< -o $@

run: $(TARGET_OS)
ifeq ($(TARGET_OS), linux)
	./$(LINUX_TARGET)
else
	$(WINDOWS_TARGET)
endif

clean:
	rm -rf $(BUILD_DIR)/

.PHONY: all linux windows clean run

