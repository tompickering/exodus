#!/usr/bin/make -f

NAME=exodus
BIN=src/exodus

.PHONY: clean all debug install

all:

OBJS := $(patsubst %.cpp,%.o,$(wildcard src/*.cpp src/*/*.cpp src/*/*/*.cpp src/*/*/*/*.cpp))
DEPS = $(OBJS:%.o=%.d)
CLEAN = $(BIN) $(OBJS) $(DEPS)

INCFLAGS=-Isrc -Isrc/deps

ifeq ($(MAKECMDGOALS),steam-linux)
INCFLAGS += -Isteamworks_sdk/sdk/public
endif

ifeq ($(MAKECMDGOALS),steam-mac)
INCFLAGS += -Isteamworks_sdk/sdk/public
endif

ifeq ($(MAKECMDGOALS),steam-windows)
INCFLAGS += -Isteamworks_sdk/sdk/public
endif

LDLIBS=-lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
CXXFLAGS=-std=c++11 -Wall -Wno-reorder -Wno-class-memaccess -Wno-format-truncation -pedantic -DMONO -DSDL $(INCFLAGS)
DBGFLAGS=-g -DDBG

#CXX=clang++
#CXXFLAGS=-Wall -Wno-reorder -Wno-class-memaccess -Wno-format-truncation -Wno-format-security -Wno-unknown-warning-option -pedantic -DMONO -DSDL $(INCFLAGS)

PREFIX = /usr/local

%.d: %.cpp
	$(CXX) $(INCFLAGS) -MM -MF $@ -MT $@ -MT $*.o $<

all: linux
all: CXXFLAGS += -O3

linux: ext=
linux: CXXFLAGS += -DLINUX
linux: bin

mac: CXX=g++-13
mac: ext=
mac: CXXFLAGS += -O3 -DMAC
mac: INCFLAGS += -I/opt/homebrew/include -L/opt/homebrew/lib
mac: bin

# Requires mingw-w64
windows: CC = x86_64-w64-mingw32-gcc
windows: CXX = x86_64-w64-mingw32-g++
windows: ext=.exe
windows: CXXFLAGS += -O3 -DWINDOWS
windows: INCFLAGS += -Iinclude -Iinclude/x86_64-linux-gnu
windows: LDLIBS := -Llib -static -l:libSDL2.dll.a -l:libSDL2_image.dll.a -l:libSDL2_mixer.dll.a -l:libSDL2_ttf.dll.a
# Replace the above line with this when building steam-windows
# windows: LDLIBS := -Llib -Lsteamworks_sdk/sdk/redistributable_bin/win64 -lsteam_api64 -static -l:libSDL2.dll.a -l:libSDL2_image.dll.a -l:libSDL2_mixer.dll.a -l:libSDL2_ttf.dll.a
windows: bin

bin: $(BIN)
	mv $(BIN)$(ext) $(NAME)$(ext)

debug: CXXFLAGS += $(DBGFLAGS)
debug: linux

STEAMFLAGS_COMMON=-DSTEAM

steam-linux: CXXFLAGS += $(STEAMFLAGS_COMMON)
steam-linux: LDLIBS += -Lsteamworks_sdk/sdk/redistributable_bin/linux64 -lsteam_api
steam-linux: linux

steam-mac: CXXFLAGS += $(STEAMFLAGS_COMMON)
steam-mac: mac

steam-windows: CXXFLAGS += $(STEAMFLAGS_COMMON)
steam-windows: windows

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
