SOURCES := $(wildcard */*.c)
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))
TARGETS := $(patsubst %.o, %  , $(OBJECTS) )
LDLIBS  := -lpthread
CCFLAGS := -Wall
CC := gcc

all: $(TARGETS)

clean:
	-rm $(TARGETS)

.PHONY: clean

.PRECIOUS: %.o

