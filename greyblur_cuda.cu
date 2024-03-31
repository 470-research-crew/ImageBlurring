
/**
* CS 470 CUDA Lab
*
* Name(s): Will Morris, Mark Myers, Lexi Krobath
*
* Originally written by William Lovo in Spring 2019 as a research project.
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "netpbm.h"
#include "timer.h"

// The size of the blur box
#define BLUR_SIZE 6

/**
* Converts a given PPM pixel array into greyscale.
* Uses the following formula for the conversion:
*      V = (R * 0.21) + (G * 0.72) + (B * 0.07)
* Where V is the grey value and RGB are the red, green, and blue values, respectively.
*/
__global__ void color_to_grey(pixel_t *in, pixel_t *out, int width, int height)
{
   int start = blockIdx.x * blockDim.x + threadIdx.x;
   int stride = blockDim.x * gridDim.x;

   for (int i = start;
       i < width * height;
       i += stride) {
       rgb_t v = (rgb_t) round(
               in[i].red * 0.21
               + in[i].green * 0.72
               + in[i].blue * 0.07);
       out[i].red = out[i].green = out[i].blue = v;
   }
}

/**
* Blurs a given PPM pixel array through box blurring.
* Strength of blur can be adjusted through the BLUR_SIZE value.
*/
__global__ void blur(pixel_t *in, pixel_t *out, int width, int height)
{
   int start = blockIdx.x * blockDim.x + threadIdx.x;
   int stride = blockDim.x * gridDim.x;

   for (int i = start; i < width * height; i+=stride) {
       int row = i / width;
       int col = i % width;
       float avg_red = 0, avg_green = 0, avg_blue = 0;
       int pixel_count = 0;

       for (int blur_row = -BLUR_SIZE; blur_row < BLUR_SIZE + 1; blur_row++) {
           for (int blur_col = -BLUR_SIZE; blur_col < BLUR_SIZE + 1; blur_col++) {
               int curr_row = row + blur_row;
               int curr_col = col + blur_col;

               if (curr_row > -1 && curr_row < height && curr_col > -1 && curr_col < width) {
                   int curr_index = curr_row * width + curr_col;
                   avg_red += in[curr_index].red;
                   avg_green += in[curr_index].green;
                   avg_blue += in[curr_index].blue;
                   pixel_count++;
               }
           }
       }

       pixel_t result = {.red   = (rgb_t) lroundf(avg_red   / pixel_count),
                         .green = (rgb_t) lroundf(avg_green / pixel_count),
                         .blue  = (rgb_t) lroundf(avg_blue  / pixel_count)
                        };
       out[i] = result;
   }
}

int main(int argc, char *argv[])
{
   if (argc != 5) {
       printf("Usage: %s <infile> <outfile> <width> <height>\n", argv[0]);
       exit(EXIT_FAILURE);
   }

   char *in = argv[1];
   char *out = argv[2];
   int width = strtol(argv[3], NULL, 10),
       height = strtol(argv[4], NULL, 10);
   long total_pixels = width * height;

   // Allocate memory for images

   ppm_t *input, *output;
   assert(cudaMallocManaged(&input, sizeof(ppm_t) + (total_pixels * sizeof(pixel_t))) == 0);
   assert(cudaMallocManaged(&output, sizeof(ppm_t) + (total_pixels * sizeof(pixel_t))) == 0);

   // Read image
   START_TIMER(read)
   read_in_ppm(in, input);
   STOP_TIMER(read)

   // Verify dimensions
   if(width != input->width || height != input->height) {
       printf("ERROR: given dimensions do not match file\n");
       exit(EXIT_FAILURE);
   }

   // Copy header to output image
   copy_header_ppm(input, output);

   // Convert to greyscale
   START_TIMER(grey)
   // LOOK INTO MAX BLOCK SIZE
   color_to_grey<<<10, 256>>>(input->pixels, output->pixels, width, height);
   cudaDeviceSynchronize();
   STOP_TIMER(grey)

   // Swap buffers in preparation for blurring
   assert(memcpy(input->pixels, output->pixels, total_pixels * sizeof(pixel_t)));

   // Apply blur filter
   START_TIMER(blur)
   blur<<<10,256>>>(input->pixels, output->pixels, width, height);
   cudaDeviceSynchronize();
   STOP_TIMER(blur)

   // Save output image
   START_TIMER(save)
   write_out_ppm(out, output);
   STOP_TIMER(save)

   // Display timing results
   printf("READ: %.6f  GREY: %.6f  BLUR: %.6f  SAVE: %.6f\n",
          GET_TIMER(read), GET_TIMER(grey), GET_TIMER(blur), GET_TIMER(save));

   assert(cudaFree(input) == 0);
   assert(cudaFree(output) == 0);

   return 0;
}

