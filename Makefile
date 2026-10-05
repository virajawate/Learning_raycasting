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
              -IC:/msys64/ucrt64/include \
              -IC:/Cpp_Libraries/imgui-sfml \
              -IC:/Cpp_Libraries/ImGuiFileDialog \
              -IC:/Cpp_Libraries/imgui

    L_FLAGS = -LC:/msys64/ucrt64/lib \
              -LC:/Cpp_Libraries/imgui-sfml/build \
              -LC:/Cpp_Libraries/imgui/build \
              -LC:/Cpp_Libraries/ImGuiFileDialog/build-mingw \
              -lImGui-SFML \
              -lImGuiFileDialog \
              -lsfml-graphics \
              -lsfml-window \
              -lsfml-audio \
              -lsfml-system \
              -lopengl32

    # Windows sources
    EXTRA_SRCS =
    EXTRA_OBJS =

else

    # ========================================================
    # Linux Configuration
    # ========================================================

    EXE =

    MKDIR = mkdir -p $(BUILD_DIR)
    RM = rm -rf $(BUILD_DIR)

    IMGUI_FILE_DIALOG_DIR := $(if $(wildcard /tmp/ImGuiFileDialog/ImGuiFileDialog.cpp),/tmp/ImGuiFileDialog,/home/viraj_awate/Deps_lib/ImGuiFileDialog)
    IMGUI_SFML_DIR := /home/viraj_awate/Deps_lib/imgui-sfml

    C_FLAGS = -std=c++17 -MMD -MP -O3 \
              -I./include \
              -I$(IMGUI_FILE_DIALOG_DIR) \
              -I/usr/local/include \
              -I/usr/include \
              -I$(IMGUI_SFML_DIR)

    L_FLAGS = -L/usr/local/lib \
              -lImGui-SFML \
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

    # ImGuiFileDialog
    EXTRA_SRCS = \
        $(IMGUI_FILE_DIALOG_DIR)/ImGuiFileDialog.cpp

    EXTRA_OBJS = \
        $(BUILD_DIR)/ImGuiFileDialog_external.o

endif

ifeq ($(DEBUG),1)
    C_FLAGS := $(filter-out -O3,$(C_FLAGS)) -O0 -g
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

SRCS = $(wildcard $(SRC_DIR)/*.cc) \
       $(wildcard $(SRC_DIR)/*.cpp)

OBJS = $(patsubst $(SRC_DIR)/%.cc,$(BUILD_DIR)/%.o,$(filter %.cc,$(SRCS))) \
       $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(filter %.cpp,$(SRCS))) \
       $(BUILD_DIR)/main.o \
       $(EXTRA_OBJS)

DEPS = $(OBJS:.o=.d)


# ============================================================
# Default Target
# ============================================================

all: build


# ============================================================
# BUILD
# ============================================================

build: $(BIN)


# ============================================================
# Link
# ============================================================

$(BIN): $(OBJS)
	$(CC) $^ -o $@ $(L_FLAGS)


# ============================================================
# Project Source Compilation
# ============================================================

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	$(MKDIR)
	$(CC) $(C_FLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(MKDIR)
	$(CC) $(C_FLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: main.cpp
	$(MKDIR)
	$(CC) $(C_FLAGS) -c $< -o $@


# ============================================================
# Linux - ImGuiFileDialog
# ============================================================

ifeq ($(OS),Windows_NT)

else

$(BUILD_DIR)/ImGuiFileDialog_external.o: $(IMGUI_FILE_DIALOG_DIR)/ImGuiFileDialog.cpp
	$(MKDIR)
	$(CC) $(C_FLAGS) -c $< -o $@

endif


# ============================================================
# Dependency Files
# ============================================================

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
# DEBUG BUILD
# ============================================================

debug:
	$(MAKE) clean
	$(MAKE) DEBUG=1 build


# ============================================================
# PHONY
# ============================================================

.PHONY: all build run clean rebuild debug