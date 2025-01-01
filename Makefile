CC = g++
C_FLAGS = -MMD -MP -c -Wall -Wextra -std=c++23 -I./include
L_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system 

BIN = raycasting
BUILD_DIR = ./build
SRC_DIR = ../src 

SRCS = $(wildcard $(SRC_DIR)/*.cc) main.cpp
OBJS = $(SRCS:%.cc=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

all: run

run: $(BUILD_DIR)/$(BIN)
	$(BUILD_DIR)/$(BIN)

$(BUILD_DIR)/$(BIN): $(OBJS)
	$(CC) $^ -o $@ $(L_FLAGS)

-include $(DEPS)

$(BUILD_DIR)/%.o: %.cc
	mkdir -p $(BUILD_DIR)
	$(CC) $(C_FLAGS) $< -o $@
