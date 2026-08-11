CC = g++

BUILD_DIR = build
SRC_DIR = src

ifeq ($(OS),Windows_NT)

    EXE = .exe

    # MSYS2 uses Bash, so use Unix-style commands.
    MKDIR = mkdir -p $(BUILD_DIR)
    RM = rm -rf $(BUILD_DIR)

    C_FLAGS = \
        -std=c++17 \
        -MMD \
        -MP \
        -O3 \
        -I./include \
        -IC:/SFML/include \
        -IC:/Cpp_Libraries/imgui \
        -IC:/Cpp_Libraries/imgui-sfml

    L_FLAGS = \
        -LC:/SFML/lib \
        -LC:/Cpp_Libraries/imgui/build \
        -LC:/Cpp_Libraries/imgui-sfml/build \
        -lImGui-SFML \
        -limgui \
        -lsfml-graphics \
        -lsfml-window \
        -lsfml-system \
        -lopengl32

else

    EXE =

    MKDIR = mkdir -p $(BUILD_DIR)
    RM = rm -rf $(BUILD_DIR)

    C_FLAGS = \
        -std=c++17 \
        -MMD \
        -MP \
        -O3 \
        -I./include \
        -I/usr/local/include \
        -I/usr/local/include/imgui

    L_FLAGS = \
        -L/usr/local/lib \
        -lImGui-SFML \
        -limgui \
        -lsfml-graphics \
        -lsfml-window \
        -lsfml-system \
        -lGL \
        -lX11 \
        -lXrandr \
        -lXcursor \
        -lXi \
        -lXinerama \
        -ludev \
        -lfreetype \
        -lpthread \
        -ldl

endif


# ============================================================
# Source files
# ============================================================

SRCS = $(wildcard $(SRC_DIR)/*.cc)

OBJS = \
    $(SRCS:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%.o) \
    $(BUILD_DIR)/main.o

DEPS = $(OBJS:.o=.d)


# ============================================================
# Default target
# ============================================================

all: build


# ============================================================
# Build executable
# ============================================================

build: $(BUILD_DIR)/raycasting$(EXE)


$(BUILD_DIR)/raycasting$(EXE): $(OBJS)
	$(CC) $^ -o $@ $(L_FLAGS)


# ============================================================
# Build directory
# ============================================================

$(BUILD_DIR):
	$(MKDIR)


# ============================================================
# Compile source files
# ============================================================

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc | $(BUILD_DIR)
	$(CC) $(C_FLAGS) -c $< -o $@


$(BUILD_DIR)/main.o: main.cpp | $(BUILD_DIR)
	$(CC) $(C_FLAGS) -c $< -o $@


# ============================================================
# Dependency files
# ============================================================

-include $(DEPS)


# ============================================================
# Clean
# ============================================================

clean:
	$(RM)


# ============================================================
# Rebuild
# ============================================================

rebuild: clean all


# ============================================================
# Phony targets
# ============================================================

.PHONY: all build clean rebuild