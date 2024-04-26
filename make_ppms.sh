# Define the dimensions of the image
width=200
height=100

# Output file name
output_file="output_serial.ppm"

# Write PPM header to the file
echo "P3" > $output_file
echo "$width $height" >> $output_file
echo "255" >> $output_file

# Generate image data (here, a gradient from red to blue)
for ((y=0; y<$height; y++)); do
    for ((x=0; x<$width; x++)); do
        r=$(($x * 255 / $width))
        g=0
        b=$(($(($height - $y)) * 255 / $height))
        echo "$r $g $b" >> $output_file
    done
done

cp output_serial.ppm output_pthread.ppm
cp output_serial.ppm output_openmp.ppm
cp output_serial.ppm output_cuda.ppm
cp output_serial.ppm output_raja.ppm
