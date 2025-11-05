# CatRoad - Makefile simples (Linux/macOS/Windows via MSYS2 MinGW64)

APP      := catroad
BUILD    := build
SRC_DIR  := src
INC_DIR  := include
SRCS     := $(wildcard $(SRC_DIR)/*.c)
OBJS     := $(SRCS:$(SRC_DIR)/%.c=$(BUILD)/%.o)
CC       := gcc
CFLAGS   := -O2 -Wall -Wextra -I$(INC_DIR)

UNAME_S := $(shell uname -s)

# libs por SO
ifeq ($(UNAME_S), Linux)
    LDFLAGS := -lraylib -lm -lpthread -ldl -lrt -lX11
else ifeq ($(UNAME_S), Darwin) # macOS
    LDFLAGS := -lraylib -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
else
    # Assume Windows (MSYS2 MinGW64)
    LDFLAGS := -lraylib -lopengl32 -lgdi32 -lwinmm
endif

$(BUILD)/$(APP): $(OBJS)
	@mkdir -p $(BUILD)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run clean
run: $(BUILD)/$(APP)
	./$(BUILD)/$(APP)

clean:
	rm -rf $(BUILD)
