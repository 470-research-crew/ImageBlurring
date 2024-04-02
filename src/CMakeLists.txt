# Adds all files to be run

add_executable(cuda cuda/greyblur_cuda.cu)
add_executable(serial serial/greyblur_serial_cpp.cpp)
add_executable(openmp openmp/greyblur_openmp.cpp)
add_executable(pthread pthread/greyblur_pthread.cpp)
