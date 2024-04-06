# set up environment
git clone --recursive git@github.com:LLNL/RAJA.git
mkdir RAJA-build
mkdir RAJA-install
mkdir RAJA-example
echo 'Success in environment set up'


# build RAJA 
cd RAJA-build
cmake -DCMAKE_INSTALL_PREFIX=../RAJA-install -DENABLE_OPENMP=On ../RAJA
make -j && make install
echo 'Success in build'

cd ../

# make and run program
cp ./example/serial.cpp ./RAJA-example
cp ./example/cuda.cu ./RAJA-example
cp ./example/openmp.cpp ./RAJA-example
cp ./example/pthread.cpp ./RAJA-example
cp ./example/raja.cpp ./RAJA-example
cp ./example/Makefile ./RAJA-example
cd ./RAJA-example
make
../run.sh ../wilson2.jpg
echo 'Success in program run'