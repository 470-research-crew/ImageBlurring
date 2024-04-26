/**
* CS 470 Research Project
*
* Name(s): Will Morris, Mark Myers, Lexi Krobath
*
* Originally written by William Lovo in Spring 2019 as a research project.
*/

#include <iostream>
#include <cmath>
#include "../netpbm.h"
#include "../timer.h"


#ifdef _OPENMP
#include <omp.h>
#endif


// The size of the blur box
#define BLUR_SIZE 6

/**
 * Converts a given PPM pixel array into greyscale.
 * Uses the following formula for the conversion:
 *      V = (R * 0.21) + (G * 0.72) + (B * 0.07)
 * Where V is the grey value and RGB are the red, green, and blue values, respectively.
 */
void color_to_grey(pixel_t *in, pixel_t *out, int width, int height)
{
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int idx = i * width + j;
            out[idx].red = out[idx].green = out[idx].blue = (rgb_t) round(
                in[idx].red * 0.21 + in[idx].green * 0.72 + in[idx].blue * 0.07);
        }
    }
}

/**
 * Blurs a given PPM pixel array through box blurring.
 * Strength of blur can be adjusted through the BLUR_SIZE value.
 */
void blur(pixel_t *in, pixel_t *out, int width, int height)
{
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int idx = i * width + j;
            float avg_red = 0, avg_green = 0, avg_blue = 0;
            int pixel_count = 0;

            for (int blur_row = -BLUR_SIZE; blur_row < BLUR_SIZE + 1; blur_row++) {
                for (int blur_col = -BLUR_SIZE; blur_col < BLUR_SIZE + 1; blur_col++) {
                    int curr_row = i + blur_row;
                    int curr_col = j + blur_col;

                    if (curr_row >= 0 && curr_row < height && curr_col >= 0 && curr_col < width) {
                        int curr_index = curr_row * width + curr_col;
                        avg_red += in[curr_index].red;
                        avg_green += in[curr_index].green;
                        avg_blue += in[curr_index].blue;
                        pixel_count++;
                    }
                }
            }

            out[idx].red = (rgb_t) round(avg_red / pixel_count);
            out[idx].green = (rgb_t) round(avg_green / pixel_count);
            out[idx].blue = (rgb_t) round(avg_blue / pixel_count);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <infile> <outfile> <width> <height>\n";
        exit(EXIT_FAILURE);
    }

    char *in = argv[1];
    char *out = argv[2];
    int width = std::stoi(argv[3]),
        height = std::stoi(argv[4]);
    long total_pixels = width * height;

    // Allocate memory for images
    ppm_t *input  = (ppm_t*)malloc(sizeof(ppm_t) + (total_pixels * sizeof(pixel_t)));
    ppm_t *output = (ppm_t*)malloc(sizeof(ppm_t) + (total_pixels * sizeof(pixel_t)));

    // Read image
    START_TIMER(read)
    read_in_ppm(in, input);
    STOP_TIMER(read)

    // Verify dimensions
    if(width != input->width || height != input->height) {
        std::cerr << "ERROR: given dimensions do not match file\n";
        exit(EXIT_FAILURE);
    }

    // Copy header to output image
    copy_header_ppm(input, output);

    // Convert to greyscale
    START_TIMER(grey)
    color_to_grey(input->pixels, output->pixels, width, height);
    STOP_TIMER(grey)

    // Swap buffers in preparation for blurring
    memcpy(input->pixels, output->pixels, total_pixels * sizeof(pixel_t));

    // Apply blur filter
    START_TIMER(blur)
    blur(input->pixels, output->pixels, width, height);
    STOP_TIMER(blur)

    // Save output image
    START_TIMER(save)
    write_out_ppm(out, output);
    STOP_TIMER(save)

    // Display timing results
    std::cout << "GREY: " << GET_TIMER(grey) << "  BLUR: " << GET_TIMER(blur) << '\n';

    free(input);
    free(output);

    return 0;
}
