TARGET = pong_game
OBJS = build/main.o build/visuals.o build/render.o build/gameLogic.o build/mathHelpers.o build/inputHelper.o

PSPDEV=$(shell psp-config --pspdev-path)
INCDIR = $(PSPDEV)/psp/include
CFLAGS = -O2 -G0 -Wall -Iinclude
CXXFLAGS = $(CFLAGS)
ASFLAGS = $(CFLAGS)

PSP_FW_VERSION = 661
PSP_LARGE_MEMORY = 0

LIBDIR = $(PSPDEV)/psp/lib
LDFLAGS =
LIBS = -lpspgu -lpspgum
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Pong

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

build/%.o: src/%.c | build
	psp-gcc $(CFLAGS) -I$(INCDIR) -I$(PSPSDK)/include -c $< -o $@

build:
	mkdir -p build