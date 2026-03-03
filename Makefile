TARGET = cheatengine

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

SRCS = main.c $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SRCS)))

CC = gcc
CFLAGS = -Wall -Wextra -g -I$(INC_DIR)

TEST_SRC = test.c
TEST_TARGET = test

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_SRC)
	$(CC) -Wall -g -o $(TEST_TARGET) $(TEST_SRC)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)

re: clean all

.PHONY: all clean re test
