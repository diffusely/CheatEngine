TARGET = cheatengine

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

SRCS_C = main.c $(wildcard $(SRC_DIR)/*.c)
SRCS_CPP = $(wildcard $(SRC_DIR)/*.cpp)
OBJS_C = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SRCS_C)))
OBJS_CPP = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(notdir $(SRCS_CPP)))
OBJS = $(OBJS_C) $(OBJS_CPP)

CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -g -I$(INC_DIR)
CXXFLAGS = -Wall -Wextra -g -I$(INC_DIR)

TEST_SRC = test.c
TEST_TARGET = test

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TEST_SRC)
	$(CC) -Wall -g -o $(TEST_TARGET) $(TEST_SRC)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)

re: clean all

.PHONY: all clean re test
