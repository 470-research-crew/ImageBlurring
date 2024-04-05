#!/usr/bin/env bash
# Build RAJA with the correct configuration options
# Will Morris

echo "Warning! This is a potential destructive action."
echo "Please execute this script in your RAJA install directory."
echo "The current directory is: `pwd`"
echo "Please type y to confirm, anything else to quit."

read confirm
if [[ "$confirm" != "y" ]]; then
  echo "abort."
  exit 0
fi

build_dir="build"
# install directory within build directory
install_dir="applications/raja"

# Start with a clean build directory
mkdir -p $build_dir && cd $build_dir && rm -r * && mkdir -p $install_dir
# Must explicitly tell RAJA to build with support for targeted backends

make="make"
enabled="-DENABLE_OPENMP=On"

# If we've got a cluster, then make that bad boy
if which srun &> /dev/null; then
  # If slurm is configured to support gpu nodes, attempt to build with cuda support
  if scontrol show nodes | grep gpu &> /dev/null; then
    make="srun --gres=gpu make -j 4"
    enabled="-DENABLE_OPENMP=On -DENABLE_CUDA=On"
  else
    make="srun make -j 4"
  fi
fi

cmake -DCMAKE_INSTALL_PREFIX=$install_dir -DENABLE_TESTS=Off $enabled ../ && $make && make install