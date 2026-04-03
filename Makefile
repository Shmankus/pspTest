TARGET = hello_world_centered
OBJS = src/main.o

INCDIR =
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS)
ASFLAGS = $(CFLAGS)

PSP_FW_VERSION = 661

LIBDIR =
LDFLAGS =
LIBS = -lpspdisplay -lpspctrl -lpspkernel -lpspgu -lpspgum -lpspge

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Hello World Centered
MKSFOEXTRA = -d MEMSIZE=0

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
