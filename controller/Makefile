
#OS = LINUX
OS = MACOSX
#OS = WINDOWS

PROG = controller

ifeq ($(OS), LINUX)
TARGET = $(PROG)
CC = gcc
STRIP = strip
CFLAGS = -Wall -O2 -DOS_$(OS)
LIBS = -lusb
else ifeq ($(OS), MACOSX)
TARGET = $(PROG).dmg
SDK = /Developer/SDKs/MacOSX10.5.sdk
ARCH =-arch x86_64
CC = gcc
STRIP = strip
CFLAGS = -Wall -O2 -DOS_$(OS) -isysroot $(SDK) $(ARCH)
LIBS = $(ARCH) -Wl,-syslibroot,$(SDK) -framework IOKit -framework CoreFoundation
else ifeq ($(OS), WINDOWS)
TARGET = $(PROG).exe
CC = i586-mingw32msvc-gcc
STRIP = i586-mingw32msvc-strip
CFLAGS = -Wall -O2 -DOS_$(OS)
LIBS = -lhid -lsetupapi
endif

OBJS = $(PROG).o hid.o font.o snake.o cube.o cursor.o misc.o


all: $(TARGET)

$(PROG): $(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(LIBS)
	$(STRIP) $(PROG)

$(PROG).exe: $(PROG)
	cp $(PROG) $(PROG).exe

$(PROG).dmg: $(PROG)
#	rm -rf tmp
#	mkdir tmp
#	cp $(PROG) tmp
#	hdiutil create -ov -volname "Raw HID Test" -srcfolder tmp $(PROG).dmg

hid.o: hid_$(OS).c hid.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(PROG) $(PROG).exe $(PROG).dmg
	rm -rf tmp

