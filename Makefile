TARGET := pokemon_world
BUILD := build
SOURCES := $(wildcard source/*.c)
OBJECTS := $(SOURCES:source/%.c=$(BUILD)/%.o)

all:
	@echo "3DS build setup pending"

clean:
	rm -rf $(BUILD)
