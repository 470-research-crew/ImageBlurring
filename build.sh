#!/usr/bin/env bash

if [ ! -d "cmake-dir" ]; then
    mkdir cmake-dir
fi

cd cmake-dir
cmake ../CMakeLists.txt
make
