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
CXXFLAGS=-Wall -pedantic -DSDL -DLINUX $(INCFLAGS)
DBGFLAGS=-g -DDBG

PREFIX = /usr/local

%.d: %.cpp
	$(CXX) $(INCFLAGS) -MM -MF $@ -MT $@ -MT $*.o $<

all: $(BIN)
	mv $(BIN) $(NAME)

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
