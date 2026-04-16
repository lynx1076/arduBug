BUILD_DIR = .build

TARGET = arduBug

LINUX_TARGET = $(BUILD_DIR)/linux/$(TARGET)
WINDOWS_TARGET = $(BUILD_DIR)/windows/$(TARGET).exe

DEBUG ?= 0
DEBUG_FLAG =
OPT_FLAG = -O3

ifeq ($(DEBUG), 1)
	DEBUG_FLAG = -g
	OPT_FLAG = -O0
endif

TARGET_OS ?= linux

LINUX_CC = gcc
WINDOWS_CC = x86_64-w64-mingw32-gcc

# Library compilation flags
LIB_OPT_FLAG = -O3
LIB_DEBUG_FLAG =

SRCS = \
	src/main.c \
	src/vector.c \
	src/microui_bindings.c \
	src/serial.c \
	src/serial_handler.c \
	src/ui.c \
	src/utils.c \

INCLUDES = \
	include \
	../protocol \
	libs/raylib/src \
	libs/raylib/src/external/glfw/include \
	libs/microui/src \

# Raylib library targets
RAYLIB_LIB_LINUX = $(BUILD_DIR)/raylib/linux/libraylib.a
RAYLIB_LIB_WINDOWS = $(BUILD_DIR)/raylib/windows/libraylib.a

# Microui library targets
MICROUI_LIB_LINUX = $(BUILD_DIR)/microui/linux/libmicroui.a
MICROUI_LIB_WINDOWS = $(BUILD_DIR)/microui/windows/libmicroui.a

# Raylib source files
RAYLIB_SRCS = \
	libs/raylib/src/rcore.c \
	libs/raylib/src/rshapes.c \
	libs/raylib/src/rtextures.c \
	libs/raylib/src/rtext.c \
	libs/raylib/src/rmodels.c \
	libs/raylib/src/raudio.c \
	libs/raylib/src/rglfw.c \

# Raylib object files
RAYLIB_LINUX_OBJS = $(RAYLIB_SRCS:%.c=$(BUILD_DIR)/raylib/linux/%.o)
RAYLIB_WINDOWS_OBJS = $(RAYLIB_SRCS:%.c=$(BUILD_DIR)/raylib/windows/%.o)

# Microui source files
MICROUI_SRCS = \
	libs/microui/src/microui.c \

# Microui object files
MICROUI_LINUX_OBJS = $(MICROUI_SRCS:%.c=$(BUILD_DIR)/microui/linux/%.o)
MICROUI_WINDOWS_OBJS = $(MICROUI_SRCS:%.c=$(BUILD_DIR)/microui/windows/%.o)

# Flags
LINUX_CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=199309L -D_DEFAULT_SOURCE -D_GLFW_X11 -DPLATFORM_DESKTOP $(addprefix -I,$(INCLUDES)) $(OPT_FLAG) $(DEBUG_FLAG)
WINDOWS_CFLAGS = -Wall -Wextra -std=c11 -DPLATFORM_DESKTOP $(addprefix -I,$(INCLUDES)) $(OPT_FLAG) $(DEBUG_FLAG)

# Library compilation flags (optimized, no debug symbols)
LINUX_LIB_CFLAGS = -Wall -Wextra -std=c11 -D_GLFW_X11 -DPLATFORM_DESKTOP $(addprefix -I,$(INCLUDES)) $(LIB_OPT_FLAG) $(LIB_DEBUG_FLAG)
WINDOWS_LIB_CFLAGS = -Wall -Wextra -std=c11 -DPLATFORM_DESKTOP $(addprefix -I,$(INCLUDES)) $(LIB_OPT_FLAG) $(LIB_DEBUG_FLAG)

LINUX_LDFLAGS = \
	-lm \
	-pthread \
	-lGL \
	-lX11 \
	-lXrandr \
	-lXcursor \
	-lXi \
	-ldl \
	$(RAYLIB_LIB_LINUX) \
	$(MICROUI_LIB_LINUX) \

WINDOWS_LDFLAGS = \
	-pthread \
	$(RAYLIB_LIB_WINDOWS) \
	$(MICROUI_LIB_WINDOWS) \
	-lopengl32 \
	-lgdi32 \
	-lwinmm \
	-lsetupapi \
	-luuid \

LINUX_OBJS = \
	$(SRCS:%.c=$(BUILD_DIR)/linux/%.o) \

WINDOWS_OBJS = \
	$(SRCS:%.c=$(BUILD_DIR)/windows/%.o) \

all: $(TARGET_OS)
ifeq ($(DEBUG), 1)
	echo "Compiling with debug symbols"
endif

linux: $(LINUX_TARGET)

windows: $(WINDOWS_TARGET)

$(LINUX_TARGET): $(LINUX_OBJS) $(RAYLIB_LIB_LINUX) $(MICROUI_LIB_LINUX)
	$(LINUX_CC) $(LINUX_OBJS) -o $@ $(LINUX_LDFLAGS)

$(WINDOWS_TARGET): $(WINDOWS_OBJS) $(RAYLIB_LIB_WINDOWS) $(MICROUI_LIB_WINDOWS)
	$(WINDOWS_CC) $(WINDOWS_OBJS) -o $@ $(WINDOWS_LDFLAGS)

# Raylib library targets
$(RAYLIB_LIB_LINUX): $(RAYLIB_LINUX_OBJS)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

$(RAYLIB_LIB_WINDOWS): $(RAYLIB_WINDOWS_OBJS)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

# Microui library targets
$(MICROUI_LIB_LINUX): $(MICROUI_LINUX_OBJS)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

$(MICROUI_LIB_WINDOWS): $(MICROUI_WINDOWS_OBJS)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

# Raylib object compilation rules
$(BUILD_DIR)/raylib/linux/%.o: %.c
	@mkdir -p $(dir $@)
	$(LINUX_CC) $(LINUX_LIB_CFLAGS) -c $< -o $@

$(BUILD_DIR)/raylib/windows/%.o: %.c
	@mkdir -p $(dir $@)
	$(WINDOWS_CC) $(WINDOWS_LIB_CFLAGS) -c $< -o $@

# Microui object compilation rules
$(BUILD_DIR)/microui/linux/%.o: %.c
	@mkdir -p $(dir $@)
	$(LINUX_CC) $(LINUX_LIB_CFLAGS) -c $< -o $@

$(BUILD_DIR)/microui/windows/%.o: %.c
	@mkdir -p $(dir $@)
	$(WINDOWS_CC) $(WINDOWS_LIB_CFLAGS) -c $< -o $@

# Main project object compilation rules
$(BUILD_DIR)/linux/%.o: %.c
	@mkdir -p $(dir $@)
	$(LINUX_CC) $(LINUX_CFLAGS) -c $< -o $@

$(BUILD_DIR)/windows/%.o: %.c
	@mkdir -p $(dir $@)
	$(WINDOWS_CC) $(WINDOWS_CFLAGS) -c $< -o $@

run: linux
	./$(LINUX_TARGET)

clean:
	rm -rf $(BUILD_DIR)/linux $(BUILD_DIR)/windows
	@echo "Run clean_all to also rebuild libraries"

clean_all:
	rm -rf .build/

.PHONY: all linux windows clean clean_all run debug

