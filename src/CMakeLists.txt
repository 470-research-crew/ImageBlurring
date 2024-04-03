
set(example_depends RAJA)

if (ENABLE_OPENMP)
  set(example_depends 
    ${example_depends}
    openmp)
endif ()

if (ENABLE_CUDA)
  set(example_depends 
    ${example_depends}
    cuda)
endif ()

blt_add_executable(
  NAME serial.exe
  SOURCES serial/greyblur_serial_cpp.cpp
  DEPENDS_ON ${example_depends})

blt_add_executable(
  NAME cuda.exe
  SOURCES cuda/greyblur_cuda.cu
  DEPENDS_ON ${example_depends})

blt_add_executable(
  NAME openmp.exe
  SOURCES openmp/greyblur_openmp.cpp
  DEPENDS_ON ${example_depends})
