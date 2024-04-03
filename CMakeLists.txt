cmake_minimum_required(VERSION 3.8)

project( imageblurring LANGUAGES CXX CUDA )

set (ENABLE_TESTS Off CACHE BOOL "")
set (ENABLE_EXAMPLES Off CACHE BOOL "")
set (ENABLE_REPRODUCERS Off CACHE BOOL "")
set (ENABLE_EXERCISES Off CACHE BOOL "")
set (ENABLE_DOCUMENTATION Off CACHE BOOL "")
set (ENABLE_BENCHMARKS Off CACHE BOOL "")

set(BLT_CXX_STD "c++14" CACHE STRING "C++ version to use")
include(RAJA/blt/SetupBLT.cmake)

if (DEFINED RAJA_DIR)
  find_package(RAJA REQUIRED)
  blt_print_target_properties(TARGET RAJA)
else ()
  add_subdirectory(/nfs/home/krobatln/1Project/ImageBlurring/RAJA)
endif ()

if(ENABLE_CUDA)
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} --expt-extended-lambda")
    set_target_properties(cuda.exe PROPERTIES CUDA_ARCHITECTURES "arch_60;arch_61")
endif()

add_subdirectory(src)