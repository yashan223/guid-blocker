CC=gcc
TARGET_NAME=guidblocker

ifeq ($(OS),Windows_NT)
EXT=dll
CFLAGS=-m32 -Wall -O1 -g -mtune=core2
LFLAGS=-m32 -g -shared -static-libgcc -static-libstdc++ -Wl,--enable-stdcall-fixup
LIBS=-L.. -lcom_plugin
else
EXT=so
CFLAGS=-m32 -Wall -O1 -g -fvisibility=hidden -mtune=core2
LFLAGS=-m32 -g -shared
LIBS=
endif

all: main.c
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(LFLAGS) -o $(TARGET_NAME).$(EXT) main.o $(LIBS)

clean:
ifeq ($(OS),Windows_NT)
	del /Q *.o *.dll 2>nul
else
	rm -f *.o *.so
endif
