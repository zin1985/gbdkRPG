PROJECT := game
BUILD_DIR := build
SRC_DIR := src
SOURCES := $(wildcard $(SRC_DIR)/*.c)
TARGET := $(BUILD_DIR)/$(PROJECT).gb

# Game Boy 向けビルド
LCC := lcc
CFLAGS := -msm83:gb -Wm-yS -Wl-j

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(SOURCES) | $(BUILD_DIR)
	$(LCC) $(CFLAGS) -o $@ $(SOURCES)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
