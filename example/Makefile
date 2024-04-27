CC = gcc
CXX = g++
CFLAGS = -g -Wall --std=c99 -Wno-unknown-pragmas -O3]
CXXFLAGS = -g -Wall -std=c++11 -O3 
CXX_R_FLAGS=-I$(INC_DIR) -std=c++14 -fopenmp
NFLAGS = -ccbin $(CC) -g -O3
LIB = -lm
INC_DIR=../RAJA-install/include
LIB_DIR=../RAJA-install/lib
LIBS=-lRAJA
NONCUDA = serial pthread openmp raja
TARGETS = $(NONCUDA) cuda

all: $(TARGETS)
noncuda: $(NONCUDA)

cuda: cuda.cu
	nvcc $(NFLAGS) -o $@ $< $(LIB)

pthread: pthread.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIB) -lpthread

openmp: openmp.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIB) -fopenmp

serial: serial.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIB)

raja: raja.cpp
	$(CXX) -o $@ $^ $(CXX_R_FLAGS) -L $(LIB_DIR) $(LIBS)

.PHONY: clean

clean:
	rm -f $(TARGETS)
