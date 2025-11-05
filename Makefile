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

	UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
    PKGCF    := $(shell which pkg-config 2>/dev/null)
    ifneq ($(PKGCF),)
        RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
        RAYLIB_LIBS   := $(shell pkg-config --libs   raylib 2/ dev/null)
    endif

    CFLAGS  := -O2 -Wall -Wextra -I$(INC_DIR) $(RAYLIB_CFLAGS)
    ifneq ($(strip $(RAYLIB_LIBS)),)
        LDFLAGS := $(RAYLIB_LIBS)
    else
        # fallback (se pkg-config não achou)
        LDFLAGS := -lraylib -lm -lpthread -ldl -lrt -lX11
        # Se você compilou do fonte e instalou em /usr/local:
        CFLAGS  += -I/usr/local/include
        LDFLAGS += -L/usr/local/lib
    endif
else ifeq ($(UNAME_S), Darwin)
    LDFLAGS := -lraylib -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
else
    LDFLAGS := -lraylib -lopengl32 -lgdi32 -lwinmm
endif

