# set up environment

if [ -d "RAJA-example" ]; then
    # If the folder exists, delete it
    rm -r "RAJA-example"
fi

mkdir RAJA-example

# NOTE: only build RAJA if RAJA build dir not found.
if [[ ! -d RAJA-build ]]; then
  git clone --recursive git@github.com:LLNL/RAJA.git
  mkdir RAJA-build
  mkdir RAJA-install

  echo 'Success in environment set up'

  # build RAJA 
  cd RAJA-build
  # Disable additional cruft -- wastes time!
  cmake -DRAJA_ENABLE_TESTS=Off -DRAJA_ENABLE_EXAMPLES=Off -DRAJA_ENABLE_EXERCISES=Off -DCMAKE_INSTALL_PREFIX=../RAJA-install -DENABLE_OPENMP=On ../RAJA
  make -j && make install
  echo 'Success in build'

  cd ../
fi

# make and run program
cp ./example/serial.cpp ./RAJA-example
cp ./example/cuda.cu ./RAJA-example
cp ./example/openmp.cpp ./RAJA-example
cp ./example/pthread.cpp ./RAJA-example
cp ./example/raja.cpp ./RAJA-example
cp ./example/Makefile ./RAJA-example
cd ./RAJA-example
make

../make_ppms.sh

gcc -o dif_compare ../dif/dif.c

echo 'Running program on wilson.jpg'
for i in {1..5}; do
    echo "Test $i"
    ../run.sh ../wilson.jpg
done

echo 'Running program on wilson2.jpg'
for i in {1..5}; do
    echo "Test $i"
    ../run.sh ../wilson2.jpg
done

echo 'Running program on duke_dog.jpg'
for i in {1..5}; do
    echo "Test $i"
    ../run.sh ../duke_dog.jpg
done

echo 'Running program on duke_dog2.jpg'
for i in {1..5}; do
    echo "Test $i"
    ../run.sh ../duke_dog2.jpg
done

echo 'Success in program run'
