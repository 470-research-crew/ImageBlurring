CC = gcc
CXX = g++
CFLAGS = -g -Wall --std=c99 -Wno-unknown-pragmas -O3
CXXFLAGS = -g -Wall -std=c++11 -O3
NFLAGS = -ccbin $(CC) -g -O3
LIB = -lm
TARGETS = greyblur_serial greyblur_cuda greyblur_pthread greyblur_openmp greyblur_serial_cpp

all: $(TARGETS)

greyblur_serial: greyblur_serial.c
	$(CC) $(CFLAGS) -o $@ $< $(LIB)

greyblur_cuda: greyblur_cuda.cu
	nvcc $(NFLAGS) -o $@ $< $(LIB)

greyblur_pthread: greyblur_pthread.c
	$(CC) $(CFLAGS) -o $@ $< $(LIB) -lpthread

greyblur_openmp: greyblur_openmp.c
	$(CC) $(CFLAGS) -fopenmp -o $@ $< $(LIB) 

greyblur_serial_cpp: greyblur_serial_cpp.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIB)

clean:
	rm -f $(TARGETS)
