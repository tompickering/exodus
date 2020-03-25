#!/usr/bin/make -f

NAME=exodus
BIN=src/exodus

.PHONY: clean all install

all:

OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp */*.cpp */*/*.cpp */*/*/*.cpp))
DEPS = $(OBJS:%.o=%.d)
CLEAN = $(BIN) $(OBJS) $(DEPS)

LDLIBS=-lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
CXXFLAGS=-g -Wall -pedantic -DSDL -DLINUX

PREFIX = /usr/local

%.d: %.cpp
	$(CXX) -MM -MF $@ -MT $@ -MT $*.o $<

all: $(BIN)
	mv $(BIN) $(NAME)

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
