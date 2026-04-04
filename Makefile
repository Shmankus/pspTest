TARGET = hello_world
OBJS = src/main.o src/renderables.o

PSPDEV=$(shell psp-config --pspdev-path)
INCDIR = $(PSPDEV)/psp/include
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS)
ASFLAGS = $(CFLAGS)

PSP_FW_VERSION = 661
PSP_LARGE_MEMORY = 0

LIBDIR = $(PSPDEV)/psp/lib
LDFLAGS =
LIBS = -lpspgu -lpspgum
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Hello World Centered

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
