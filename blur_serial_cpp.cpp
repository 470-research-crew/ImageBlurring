/**
 * CS 470 CUDA Lab
 *
 * Name(s): 
 *
 * Originally written by William Lovo in Spring 2019 as a research project.
 */

#include <iostream>
#include <cmath>
#include <cstring>
#include "netpbm.h"
#include "timer.h"

// The size of the blur box
#define BLUR_SIZE 6

/**
 * Blurs a given PPM pixel array through box blurring.
 * Strength of blur can be adjusted through the BLUR_SIZE value.
 */
void blur(pixel_t *in, pixel_t *out, int width, int height)
{
    for (int i = 0; i < width * height; i++) {
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

        pixel_t result = {.red   = static_cast<rgb_t>(std::round(avg_red   / pixel_count)),
                          .green = static_cast<rgb_t>(std::round(avg_green / pixel_count)),
                          .blue  = static_cast<rgb_t>(std::round(avg_blue  / pixel_count))
                         };
        out[i] = result;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        std::cout << "Usage: " << argv[0] << " <infile> <outfile> <width> <height>\n";
        exit(EXIT_FAILURE);
    }

    char *in = argv[1];
    char *out = argv[2];
    int width = std::strtol(argv[3], NULL, 10),
        height = std::strtol(argv[4], NULL, 10);
    long total_pixels = width * height;

    // Allocate memory for images
    ppm_t *input  = reinterpret_cast<ppm_t*>(malloc(sizeof(ppm_t) + (total_pixels * sizeof(pixel_t))));
    ppm_t *output = reinterpret_cast<ppm_t*>(malloc(sizeof(ppm_t) + (total_pixels * sizeof(pixel_t))));

    // Read image
    START_TIMER(read)
    read_in_ppm(in, input);
    STOP_TIMER(read)

    // Verify dimensions
    if(width != input->width || height != input->height) {
        std::cout << "ERROR: given dimensions do not match file\n";
        exit(EXIT_FAILURE);
    }

    // Copy header to output image
    copy_header_ppm(input, output);

    // Swap buffers in preparation for blurring
    std::memcpy(input->pixels, output->pixels, total_pixels * sizeof(pixel_t));

    // Apply blur filter
    START_TIMER(blur)
    blur(input->pixels, output->pixels, width, height);
    STOP_TIMER(blur)

    // Save output image
    START_TIMER(save)
    write_out_ppm(out, output);
    STOP_TIMER(save)

    // Display timing results
    std::cout << "READ: " << GET_TIMER(read) << "  BLUR: " << GET_TIMER(blur) << "  SAVE: " << GET_TIMER(save) << "\n";

    free(input);
    free(output);

    return 0;
}
