BUILD_DIR = .build
TARGET = $(BUILD_DIR)/arduBug

CC = gcc

SRCS = \
	src/main.c \

INCLUDES = \
	-Iinclude \
	-I../protocol \

CFLAGS = -Wall -Wextra -std=c11 -g $(INCLUDES)

LDFLAGS = \
	-lm \
	-lpthread \

OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all clean run

