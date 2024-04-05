RAJA_OPTS=-DRAJA_DIR=raja/lib/cmake/raja

mkdir build
cmake ${RAJA_OPTS} build
cd build
make
