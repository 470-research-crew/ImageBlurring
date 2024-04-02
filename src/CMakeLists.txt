# Adds all files to be run

# Already specified to use CUDA
add_executable(cuda cuda/greyblur_cuda.cu netpbm.h)
add_executable(serial serial/greyblur_serial_cpp.cpp netpbm.h)

# attempt to link openmp executable with openmp;
# this executable will still work w/o openmp
add_executable(openmp openmp/greyblur_openmp.cpp netpbm.h)
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(openmp PUBLIC OpenMP::OpenMP_CXX)
endif()

find_package(Threads REQUIRED)
add_executable(pthread pthread/greyblur_pthread.cpp netpbm.h)
target_link_libraries(pthread Threads::Threads)

# Raja version
find_package(RAJA REQUIRED)
