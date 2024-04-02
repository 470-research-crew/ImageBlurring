# Adds all files to be run

# Already specified to use CUDA
add_executable(cuda cuda/greyblur_cuda.cu netpbm.h)
add_executable(serial serial/greyblur_serial_cpp.cpp netpbm.h)
add_executable(openmp openmp/greyblur_openmp.cpp netpbm.h)

find_package(Threads REQUIRED)
add_executable(pthread pthread/greyblur_pthread.cpp netpbm.h)
target_link_libraries(pthread Threads::Threads)
