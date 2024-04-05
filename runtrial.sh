#!/bin/bash
#
# To run on the cluster w/ a GPU node:
#
#   srun --gres=gpu ./run.sh <infile>
#

# Check for filename
if [ $# -ne 1 ]; then
    echo "Usage: ./run.sh <image>"
    exit 1
fi

# Runs the ImageMagick 'identify' command to extract image dimensions
dims=$(identify ${@: -1} | cut -f 3 -d ' ')
width=$(echo ${dims} | cut -f 1 -d 'x')
height=$(echo ${dims} | cut -f 2 -d 'x')

# Display help message if no dimensions are found
#if [[ -z "$dims" ]]; then
#  help=true
#fi
if [[ -z "$dims" ]]; then
  echo "ERROR: No dimensions found for the image."
  exit 1
fi

# Create a temporary file in PPM format
temp_file="converted_image.ppm"
convert ${@: -1} -compress None PPM:${temp_file}

# Run serial program if it is present
if [ -e "./build/bin/serial.exe" ]; then
    echo -n "Serial:       "
    srun ./build/bin/serial.exe ${temp_file} output_serial.ppm ${width} ${height}
    convert output_serial.ppm PNG:output_serial.png
    rm output_serial.ppm
else
    echo "ERROR: Serial program not found"
fi

if [ -e "./build/bin/openmp.exe" ]; then
    echo -n "OpenMP:       "
    srun ./build/bin/openmp.exe ${temp_file} output_openmp.ppm ${width} ${height}
    convert output_openmp.ppm PNG:output_openmp.png
    rm output_openmp.ppm
else
    echo "ERROR: OpenMP program not found"
fi

if [ -e "./build/bin/cuda.exe" ]; then
    echo -n "CUDA:         "
    srun --gres=gpu ./build/bin/cuda.exe ${temp_file} output_cuda.ppm ${width} ${height}
    convert output_cuda.ppm PNG:output_cuda.png
    rm output_cuda.ppm
else
    echo "ERROR: CUDA program not found"
fi
# Remove the temporary file
rm ${temp_file}

