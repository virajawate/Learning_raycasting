# ============================================================
# Compiler / Project
# ============================================================

CC = g++
BIN_NAME = raycasting
BUILD_DIR = build
SRC_DIR = src


# ============================================================
# Platform Configuration
# ============================================================

ifeq ($(OS),Windows_NT)

    EXE = .exe

    SHELL := cmd.exe
    .SHELLFLAGS := /C

    ifdef MSYSTEM
        MKDIR = mkdir -p $(BUILD_DIR)
        RM = rm -rf $(BUILD_DIR)
    else
        ifdef COMSPEC
            MKDIR = $(COMSPEC) /C "if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)"
            RM = $(COMSPEC) /C "if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)"
        else
            MKDIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
            RM = if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)
        endif
    endif

    C_FLAGS = -std=c++17 -MMD -MP -O3 \
              -I./include \
              -IC:/SFML/include \
              -IC:/Cpp_Libraries/imgui-sfml \
              -IC:/Cpp_Libraries/imgui

    L_FLAGS = -LC:/SFML/lib \
              -LC:/Cpp_Libraries/imgui-sfml/build \
              -LC:/Cpp_Libraries/imgui/build \
              -lImGui-SFML \
              -limgui \
              -lsfml-graphics \
              -lsfml-window \
              -lsfml-audio \
              -lsfml-system \
              -lopengl32

else

    EXE =

    MKDIR = mkdir -p $(BUILD_DIR)
    RM = rm -rf $(BUILD_DIR)

    C_FLAGS = -std=c++17 -MMD -MP -O3 \
              -I./include \
              -I/usr/local/include

    L_FLAGS = -L/usr/local/lib \
              -lsfml-graphics \
              -lsfml-window \
              -lsfml-audio \
              -lsfml-system \
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


# ============================================================
# Files
# ============================================================

ifeq ($(OS),Windows_NT)
    SEP := \\
else
    SEP := /
endif

BIN = $(BUILD_DIR)$(SEP)$(BIN_NAME)$(EXE)

SRCS = $(wildcard $(SRC_DIR)/*.cc)

OBJS = $(SRCS:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%.o) \
       $(BUILD_DIR)/main.o

DEPS = $(OBJS:.o=.d)


# ============================================================
# Default Target
# ============================================================

all: build


# ============================================================
# BUILD
# ============================================================

build: $(BIN)


# Link
$(BIN): $(OBJS)
	$(CC) $^ -o $@ $(L_FLAGS)


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	$(MKDIR)
	$(CC) $(C_FLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: main.cpp
	$(MKDIR)
	$(CC) $(C_FLAGS) -c $< -o $@


# Dependency files
-include $(DEPS)


# ============================================================
# RUN
# ============================================================

run: build
	$(BIN) $(ARGS)


# ============================================================
# CLEAN
# ============================================================

clean:
	$(RM)


# ============================================================
# REBUILD
# ============================================================

rebuild: clean build


# ============================================================
# PHONY
# ============================================================

.PHONY: all build run clean rebuild