CC=gcc
CFLAGS=-g -Wall --std=c99 -Wno-unknown-pragmas -O3
NFLAGS=-ccbin $(CC) -g -O3
LIB=-lm
TARGETS=blur_serial blur_cuda blur_pthread blur_openmp

all: $(TARGETS)

blur_serial: blur_serial.c
	$(CC) $(CFLAGS) -o $@ $< $(LIB)

blur_cuda: blur_cuda.cu
	nvcc $(NFLAGS) -o $@ $< $(LIB)

blur_pthread: blur_pthread.c
	nvcc $(NFLAGS) -o $@ $< $(LIB)

blur_openmp: blur_openmp.c
	nvcc $(NFLAGS) -o $@ $< $(LIB)

clean:
	rm -f $(TARGETS)

