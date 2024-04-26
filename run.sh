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
if [[ -z "$dims" ]]; then
  help=true
fi

# Create a temporary file in PPM format
temp_file="converted_image.ppm"
convert ${@: -1} -compress None PPM:${temp_file}


# Run serial program if it is present
if [ -e "serial" ]; then
    echo -n "Serial:       "
    srun ./serial ${temp_file} output_serial.ppm ${width} ${height}
    convert output_serial.ppm PNG:output_serial.png
    rm output_serial.ppm
else
    echo "ERROR: Serial program not found"
fi

if [ -e "pthread" ]; then
    echo -n "Pthread:      "
    srun ./pthread ${temp_file} output_pthread.ppm ${width} ${height} 16
    convert output_pthread.ppm PNG:output_pthread.png
    rm output_pthread.ppm
else
    echo "ERROR: Pthread program not found"
fi

if [ -e "openmp" ]; then
    echo -n "OpenMP:       "
    srun ./openmp ${temp_file} output_openmp.ppm ${width} ${height}
    convert output_openmp.ppm PNG:output_openmp.png
    rm output_openmp.ppm
    
else
    echo "ERROR: OpenMP program not found"
fi

#Run CUDA program if it is present
if [ -e "cuda" ]; then
    echo -n "CUDA:         "
    srun --gres=gpu ./cuda ${temp_file} output_cuda.ppm ${width} ${height}
    convert output_cuda.ppm PNG:output_cuda.png
    rm output_cuda.ppm
else
    echo "ERROR: CUDA program not found"
fi


if [ -e "raja" ]; then
    echo -n "RAJA:         "
    srun ./raja ${temp_file} output_raja.ppm ${width} ${height}
    convert output_raja.ppm PNG:output_raja.png
    rm output_raja.ppm

else
    echo "ERROR: RAJA program not found"
fi


srun ./dif_compare output_serial.png output_pthread.png
srun ./dif_compare output_serial.png output_openmp.png
srun ./dif_compare output_serial.png output_cuda.png
srun ./dif_compare output_serial.png output_raja.png


# Remove the temporary file
rm ${temp_file}

