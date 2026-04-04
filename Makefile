TARGET = hello_world
OBJS = src/main.o

INCDIR =
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS)
ASFLAGS = $(CFLAGS)

PSP_FW_VERSION = 661
PSP_LARGE_MEMORY = 0

LIBDIR =
LDFLAGS =
LIBS = -lpspgu -lpspgum
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Hello World Centered

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
