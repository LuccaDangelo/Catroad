# CatRoad - Makefile robusto (Linux)
APP      := catroad
BUILD    := build
SRC_DIR  := src
INC_DIR  := include
SRCS     := $(wildcard $(SRC_DIR)/*.c)
OBJS     := $(SRCS:$(SRC_DIR)/%.c=$(BUILD)/%.o)

CC       := gcc
CFLAGS   := -O2 -Wall -Wextra -I$(INC_DIR)

# --- Ajuste automático: tenta pkg-config, senão usa /usr/local como fallback ---
RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
RAYLIB_LIBS   := $(shell pkg-config --libs   raylib 2>/dev/null)

# Força incluir /usr/local se compilou a raylib do fonte (make install padrão)
CFLAGS  += $(RAYLIB_CFLAGS) -I/usr/local/include
LDFLAGS := -Wl,--no-as-needed -L/usr/local/lib

# Ordem IMPORTA: primeiro objetos, depois raylib, depois libm e cia.
# (Mesmo que pkg-config retorne algo, completamos com -lm e libs de sistema.)
LDLIBS  := $(RAYLIB_LIBS) -lraylib -lm -lpthread -ldl -lrt -lX11

$(BUILD)/$(APP): $(OBJS)
	@mkdir -p $(BUILD)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(BUILD)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run clean
run: $(BUILD)/$(APP)
	./$(BUILD)/$(APP)

clean:
	rm -rf $(BUILD)
