FILTER  := bo/bo.c bo/bo2.c bo/bo3.c bo/bo4.c bo/vector.c bo/vector2.c bo/vector3.c bo/vector4.c
SOURCES := $(filter-out $(FILTER),$(wildcard */*.c) )
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))
TARGETS := $(patsubst %.o, %  , $(OBJECTS) )
LDLIBS  := -lpthread -lrt
CFLAGS := -Wall -O3
CC := gcc

all: $(TARGETS)

clean:
	-rm $(TARGETS)

.PHONY: clean

.PRECIOUS: %.o

