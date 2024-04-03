#!/usr/bin/env bash

if [ ! -d "cmake-dir" ]; then
    mkdir cmake-dir
fi

cd cmake-dir
cmake -DRAJA_DIR=/nfs/home/krobatln/1Project/ImageBlurring/RAJA/share/raja/cmake -DCMAKE_INSTALL_PREFIX=/nfs/home/krobatln/1Project/ImageBlurring/ ../
#cmake ../CMakeLists.txt
make
