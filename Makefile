CC = g++
C_FLAGS = -MMD -MP -O3 -I./include
L_FLAGS = -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system -lGL

BIN = raycasting
BUILD_DIR = ./build
SRC_DIR = ./src

SRCS = $(wildcard $(SRC_DIR)/*.cc)
OBJS = $(SRCS:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%.o) $(BUILD_DIR)/main.o
DEPS = $(OBJS:.o=.d)

all: run

run: $(BUILD_DIR)/$(BIN)
	$(BUILD_DIR)/$(BIN) $(ARGS)

$(BUILD_DIR)/$(BIN): $(OBJS)
	$(CC) $^ -o $@ $(L_FLAGS)

-include $(DEPS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	mkdir -p $(BUILD_DIR)
	$(CC) $(C_FLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: main.cpp
	$(CC) $(C_FLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY : all run clean

