CC = g++
BIN = raycasting$(EXE)
BUILD_DIR = build
SRC_DIR = src

ifeq ($(OS),Windows_NT)
    MKDIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
    RM = if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)
    EXE = .exe

    C_FLAGS = -std=c++17 -MMD -MP -O3 -I./include -IC:/SFML/include
    L_FLAGS = -LC:/SFML/lib \
              -lsfml-graphics \
              -lsfml-window \
              -lsfml-audio \
              -lsfml-system \
              -lopengl32
else
    MKDIR = mkdir -p $(BUILD_DIR)
    RM = rm -rf $(BUILD_DIR)
    EXE =

    C_FLAGS = -std=c++17 -MMD -MP -O3 -I./include -I/usr/local/include
	L_FLAGS = \
		-L/usr/local/lib \
		-lsfml-graphics-s \
		-lsfml-window-s \
		-lsfml-audio-s \
		-lsfml-system-s \
		-lGL \
		-lX11 \
		-lXrandr \
		-lXcursor \
		-lXi \
		-lXinerama \
		-lXxf86vm \
		-ludev \
		-lfreetype \
		-lpthread \
		-ldl

endif

SRCS = $(wildcard $(SRC_DIR)/*.cc)
OBJS = $(SRCS:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%.o) $(BUILD_DIR)/main.o
DEPS = $(OBJS:.o=.d)

all: run

run: $(BUILD_DIR)/$(BIN)
	$(BUILD_DIR)/$(BIN) $(ARGS)

$(BUILD_DIR)/$(BIN): $(OBJS)
	$(CC) $^ -o $@ $(L_FLAGS)

-include $(DEPS)

$(BUILD_DIR):
	$(MKDIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc | $(BUILD_DIR)
	$(CC) $(C_FLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: main.cpp | $(BUILD_DIR)
	$(CC) $(C_FLAGS) -c $< -o $@

clean:
	$(RM)

rebuild: clean all

.PHONY: all run clean