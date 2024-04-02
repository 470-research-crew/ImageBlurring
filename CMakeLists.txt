cmake_minimum_required(VERSION 3.5)
project(imageblurring LANGUAGES CXX CUDA)

include_directories("src")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../out)
set(CMAKE_CUDA_ARCHITECTURES all-major)

enable_testing()

# Deferring to cmake
add_subdirectory(src)
# add_subdirectory(test)
