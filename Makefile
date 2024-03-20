CC=gcc
CFLAGS=-g -Wall --std=c99 -Wno-unknown-pragmas -O3
NFLAGS=-ccbin $(CC) -g -O3
LIB=-lm
TARGETS=greyblur_serial greyblur_cuda

all: $(TARGETS)

greyblur_serial: greyblur.c
	$(CC) $(CFLAGS) -o $@ $< $(LIB)

greyblur_cuda: greyblur.cu
	nvcc $(NFLAGS) -o $@ $< $(LIB)

clean:
	rm -f $(TARGETS)

