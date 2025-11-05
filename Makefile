# CatRoad - Makefile (Linux/macOS/Windows via MSYS2)
APP      := catroad
BUILD    := build
SRC_DIR  := src
INC_DIR  := include
SRCS     := $(wildcard $(SRC_DIR)/*.c)
OBJS     := $(SRCS:$(SRC_DIR)/%.c=$(BUILD)/%.o)
CC       := gcc
CFLAGS   := -O2 -Wall -Wextra -I$(INC_DIR)

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
    # Tenta usar pkg-config; se falhar, usa fallback clássico
    RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
    RAYLIB_LIBS   := $(shell pkg-config --libs   raylib 2>/dev/null)
    CFLAGS  += $(RAYLIB_CFLAGS)
    ifneq ($(strip $(RAYLIB_LIBS)),)
        LDFLAGS := $(RAYLIB_LIBS)
    else
        LDFLAGS := -lraylib -lm -lpthread -ldl -lrt -lX11
        # Descomente se você instalou localmente em /usr/local
        # CFLAGS  += -I/usr/local/include
        # LDFLAGS += -L/usr/local/lib
    endif
else ifeq ($(UNAME_S), Darwin)
    LDFLAGS := -lraylib -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
else
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
