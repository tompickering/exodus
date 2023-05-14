#!/usr/bin/make -f

NAME=exodus
BIN=src/exodus

.PHONY: clean all debug install

all:

OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp */*.cpp */*/*.cpp */*/*/*.cpp))
DEPS = $(OBJS:%.o=%.d)
CLEAN = $(BIN) $(OBJS) $(DEPS)

INCFLAGS=-Isrc
LDLIBS=-lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
CXXFLAGS=-Wall -Wno-reorder -pedantic -DSDL $(INCFLAGS)
DBGFLAGS=-g -DDBG

PREFIX = /usr/local

%.d: %.cpp
	$(CXX) $(INCFLAGS) -MM -MF $@ -MT $@ -MT $*.o $<

all: linux

linux: ext=
linux: CXXFLAGS += -DLINUX
linux: bin

# Requires mingw-w64
windows: CXX = x86_64-w64-mingw32-g++
windows: ext=.exe
windows: CXXFLAGS += -DWINDOWS
windows: INCFLAGS += -Iinclude -Iinclude/x86_64-linux-gnu
windows: LDLIBS := -Llib -static -l:libSDL2.dll.a -l:libSDL2_image.dll.a -l:libSDL2_mixer.dll.a -l:libSDL2_ttf.dll.a
windows: bin

bin: $(BIN)
	mv $(BIN)$(ext) $(NAME)$(ext)

debug: CXXFLAGS += $(DBGFLAGS)
debug: all

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
