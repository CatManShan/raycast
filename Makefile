CC     = gcc
OPTIMIZATION = -O3
CFLAGS = -Wall -Wextra -Wpedantic -std=c99 $(OPTIMIZATION)
LIBS   = -pthread -lm

DEPS = src/simpcg/simpcg.h \
       src/option-map/option-map.h \
       $(DEBUG_DEPS)

OBJS = obj/raycast.o \
       obj/scg-buffer.o \
       obj/scg-pixel-buffer.o \
       obj/option-map.o \
       $(DEBUG_OBJS)

DEBUG = -DNDEBUG
DEFINES = $(DEBUG)

all: make-dirs bin/raycast

debug:
	make all DEBUG_DEPS=src/mem-utils/mem-debug.h DEBUG_OBJS=obj/mem-debug.o OPTIMIZATION=-g DEBUG=-DMEM_DEBUG

make-dirs: obj/ bin/

obj/:
	if [ ! -d obj ]; then mkdir obj; fi

bin/:
	if [ ! -d bin ]; then mkdir bin; fi

# raycast-test

bin/raycast: $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(DEFINES)

obj/raycast.o: src/raycast.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DEFINES)

# simpcg

obj/scg-buffer.o: src/simpcg/scg-buffer.c src/simpcg/simpcg.h $(DEBUG_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DEFINES)

obj/scg-pixel-buffer.o: src/simpcg/scg-pixel-buffer.c src/simpcg/simpcg.h $(DEBUG_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DEFINES)

# option-map

obj/option-map.o: src/option-map/option-map.c src/option-map/option-map.h $(DEBUG_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DEFINES)

# mem-debug

obj/mem-debug.o: src/mem-utils/mem-debug.c src/mem-utils/mem-debug.h
	$(CC) -c -o $@ $< $(CFLAGS) $(DEFINES)

#clean

clean:
	rm -rf obj/*

.PHONY: release debug make-dirs clean

