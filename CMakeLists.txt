cmake_minimum_required(VERSION 3.5)
project(imageblurring)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../out)

enable_testing()

# Deferring to cmake
add_subdirectory(src)
add_subdirectory(test)