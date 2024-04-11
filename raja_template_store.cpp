//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2016-24, Lawrence Livermore National Security, LLC
// and RAJA project contributors. See the RAJA/LICENSE file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#include <cstdlib>
#include <iostream>
#include <iomanip>

#include "../timer.h"
#include "RAJA/RAJA.hpp"

#if defined(RAJA_ENABLE_CUDA)
const int CUDA_BLOCK_SIZE = 256;
#endif

int main(int argc, char *argv[])
{

  std::cout << "\n\nExercise #2: Approximate pi using a Riemann sum...\n";

  const int N = 5120 * 5120;
  const double dx = 1.0 / double(N); 

  int prec = 16;

    START_TIMER(c)
  std::cout << "\n Running C-style sequential pi approximation...\n";
 
  double c_pi = 0.0;

  for (int i = 0; i < N; ++i) {
      double x = (double(i) + 0.5) * dx;
      c_pi += dx / (1.0 + x * x);
  }
  c_pi *= 4.0;

  std::cout << "\tpi = " << std::setprecision(prec) 
            << c_pi << std::endl;

    STOP_TIMER(c)
//----------------------------------------------------------------------------//
// RAJA sequential variant.
//----------------------------------------------------------------------------//

        START_TIMER(seqraja)
  std::cout << "\n Running RAJA sequential pi approximation...\n";

  using EXEC_POL1   = RAJA::seq_exec;
  using REDUCE_POL1 = RAJA::seq_reduce; 

  RAJA::ReduceSum< REDUCE_POL1, double > seq_pi(0.0);

  RAJA::forall< EXEC_POL1 >(RAJA::RangeSegment(0, N), [=](int i) {
      double x = (double(i) + 0.5) * dx;
      seq_pi += dx / (1.0 + x * x);
  });
  double seq_pi_val = seq_pi.get() * 4.0;

  std::cout << "\tpi = " << std::setprecision(prec) 
            << seq_pi_val << std::endl;

    STOP_TIMER(seqraja)
//----------------------------------------------------------------------------//
// C-style OpenMP multithreading variant.
//----------------------------------------------------------------------------//

    START_TIMER(openmp)
#if defined(RAJA_ENABLE_OPENMP)

  std::cout << "\n Running C-style OpenMP vector addition...\n";

  double c_pi_omp = 0.0;

  #pragma omp parallel for reduction(+:c_pi_omp)
  for (int i = 0; i < N; ++i) {
      double x = (double(i) + 0.5) * dx;
      c_pi_omp += dx / (1.0 + x * x);
  }
  c_pi_omp *= 4.0;

  std::cout << "\tpi = " << std::setprecision(prec)
            << c_pi_omp << std::endl;

#endif
    STOP_TIMER(openmp)

//----------------------------------------------------------------------------//
// RAJA OpenMP multithreading variant.
//----------------------------------------------------------------------------//
    START_TIMER(openmpraja)
#if defined(RAJA_ENABLE_OPENMP)

  std::cout << "\n Running RAJA OpenMP pi approximation...\n";

  using EXEC_POL2   = RAJA::omp_parallel_for_exec;
  using REDUCE_POL2 = RAJA::omp_reduce;

  RAJA::ReduceSum< REDUCE_POL2, double > omp_pi(0.0);

  RAJA::forall< EXEC_POL2 >(RAJA::RangeSegment(0, N), [=](int i) {
      double x = (double(i) + 0.5) * dx;
      omp_pi += dx / (1.0 + x * x);
  });
  double omp_pi_val = omp_pi.get() * 4.0;

  std::cout << "\tpi = " << std::setprecision(prec)
            << omp_pi_val << std::endl;

#endif
STOP_TIMER(openmpraja)

//----------------------------------------------------------------------------//
// RAJA CUDA variant.
//----------------------------------------------------------------------------//
START_TIMER(cudaraja)
#if defined(RAJA_ENABLE_CUDA)

  std::cout << "\n Running RAJA CUDA pi approximation...\n";

  using EXEC_POL3   = RAJA::cuda_exec<CUDA_BLOCK_SIZE>;
  using REDUCE_POL3 = RAJA::cuda_reduce;

  RAJA::ReduceSum< REDUCE_POL3, double > cuda_pi(0.0);

  RAJA::forall< EXEC_POL3 >(RAJA::RangeSegment(0, N), [=] RAJA_DEVICE (int i) {
      double x = (double(i) + 0.5) * dx;
      cuda_pi += dx / (1.0 + x * x);
  });
  double cuda_pi_val = cuda_pi.get() * 4.0;

  std::cout << "\tpi = " << std::setprecision(prec)
            << cuda_pi_val << std::endl;

#endif
STOP_TIMER(cudaraja)

std::cout << "C: " << GET_TIMER(c) << "  SEQ_RAJA: " << GET_TIMER(seqraja) << "  OPENMP: " << GET_TIMER(openmp) << "  OPENMP_RAJA: " << GET_TIMER(openmpraja) << "  CUDA_RAJA: " << GET_TIMER(cudaraja) << std::endl;
  std::cout << "\n DONE!...\n";
  

  return 0;
}
