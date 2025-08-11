#!/usr/bin/make -f

NAME=exodus
BIN=src/exodus

.PHONY: clean all debug install

all:

OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp */*.cpp */*/*.cpp */*/*/*.cpp))
DEPS = $(OBJS:%.o=%.d)
CLEAN = $(BIN) $(OBJS) $(DEPS)

INCFLAGS=-Isrc
LDLIBS=-lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lcjson
CXXFLAGS=-Wall -Wno-reorder -Wno-class-memaccess -Wno-format-truncation -pedantic -DMONO -DSDL $(INCFLAGS)
DBGFLAGS=-g -DDBG

PREFIX = /usr/local

%.d: %.cpp
	$(CXX) $(INCFLAGS) -MM -MF $@ -MT $@ -MT $*.o $<

all: linux
all: CXXFLAGS += -O3

linux: ext=
linux: CXXFLAGS += -DLINUX
linux: bin

mac: CXX=g++-13 -std=c++11
mac: ext=
mac: CXXFLAGS += -O3 -DMAC
mac: INCFLAGS += -I/opt/homebrew/include -L/opt/homebrew/lib
mac: bin

# Requires mingw-w64
windows: CXX = x86_64-w64-mingw32-g++
windows: ext=.exe
windows: CXXFLAGS += -O3 -DWINDOWS
windows: INCFLAGS += -Iinclude -Iinclude/x86_64-linux-gnu
windows: LDLIBS := -Llib -static -l:libSDL2.dll.a -l:libSDL2_image.dll.a -l:libSDL2_mixer.dll.a -l:libSDL2_ttf.dll.a
windows: bin

bin: $(BIN)
	mv $(BIN)$(ext) $(NAME)$(ext)

debug: CXXFLAGS += $(DBGFLAGS)
debug: linux

$(BIN):
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(BIN): $(OBJS)

clean:
	rm -rf $(CLEAN) core

install: all
	install $(NAME) $(DESTDIR)$(PREFIX)/bin

ifneq ($(findstring clean,$(MAKECMDGOALS)),clean)
-include $(DEPS)
endif
