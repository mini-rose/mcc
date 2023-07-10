# mcc build rules
# Copyright (c) 2023 mini-rose

CC ?= cc

CFLAGS    += -Wall -Wextra
LDFLAGS   +=
MAKEFLAGS += -j$(nproc)

PREFIX := /usr/local

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))
OUT := build/mcc
BT  ?= debug

ifeq ($(BT), debug)
	CFLAGS += -O0 -ggdb -DDEBUG=1 -fsanitize=address
else ifeq ($(BT), release)
	CFLAGS += -O3 -s
else
	$(error unknown build type: $(BT))
endif


all: build $(OUT)


build:
	mkdir -p build

clean:
	echo "  RM build"
	rm -rf build

install:
	echo "  CP $(OUT) /usr/bin/mcc"
	cp $(OUT) /usr/bin/mcc

compile_flags.txt:
	echo $(CFLAGS) | tr ' ' '\n' > compile_flags.txt

$(OUT): $(OBJ)
	@echo "  LD $@"
	@$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^

build/%.o: src/%.c
	@echo "  CC $<"
	@$(CC) -c -o $@ $(CFLAGS) $<


.PHONY: compile_flags.txt
.SILENT: build clean install
