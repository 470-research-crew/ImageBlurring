/**
 * CS 470 CUDA Lab
 *
 * Name(s): 
 *
 * Originally written by William Lovo in Spring 2019 as a research project.
 */

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "netpbm.h"
#include "timer.h"


// The size of the blur box
#define BLUR_SIZE 6
int thread_count;

struct ThreadData {
    pixel_t *in;
    pixel_t *out;
    int width;
    int height;
    int thread_id;
    int thread_count;
};

typedef struct {
    pixel_t *in;
    pixel_t *out;
    int width;
    int height;
    int start_row;
    int end_row;
} BlurThreadData;

/**
 * Converts a given PPM pixel array into greyscale.
 * Uses the following formula for the conversion:
 *      V = (R * 0.21) + (G * 0.72) + (B * 0.07)
 * Where V is the grey value and RGB are the red, green, and blue values, respectively.
 */
void color_to_grey(void* arg)
{
    struct ThreadData* data = (struct ThreadData*)arg;
    int width = data->width;
    int height = data->height;
    int thread_id = data->thread_id;
    int thread_count = data->thread_count;
    pixel_t *in = data->in;
    pixel_t *out = data->out;

    int chunk_size = (height + thread_count - 1) / thread_count;
    int start_row = thread_id * chunk_size;
    int end_row = fmin((thread_id + 1) * chunk_size, height);

    for (int row = start_row; row < end_row; row++) {
        for (int col = 0; col < width; col++) {
            int index = row * width + col;
            rgb_t v = (rgb_t)round(
                    in[index].red * 0.21 +
                    in[index].green * 0.72 +
                    in[index].blue * 0.07);
            out[index].red = out[index].green = out[index].blue = v;
        }
    }
}

/**
 * Blurs a given PPM pixel array through box blurring.
 * Strength of blur can be adjusted through the BLUR_SIZE value.
 */
void blur(void* arg)
{
    struct ThreadData* data = (struct ThreadData*)arg;
    int width = data->width;
    int height = data->height;
    int thread_id = data->thread_id;
    int thread_count = data->thread_count;
    pixel_t *in = data->in;
    pixel_t *out = data->out;

    int chunk_size = (height + thread_count - 1) / thread_count;
    int start_row = thread_id * chunk_size;
    int end_row = fmin((thread_id + 1) * chunk_size, height);

    for (int row = start_row; row < end_row; row++) {
        for (int col = 0; col < width; col++) {
            int i = row * width + col;
            float avg_red = 0, avg_green = 0, avg_blue = 0;
            int pixel_count = 0; 

            for (int blur_row = -BLUR_SIZE; blur_row < BLUR_SIZE + 1; blur_row++) {
                for (int blur_col = -BLUR_SIZE; blur_col < BLUR_SIZE + 1; blur_col++) {
                    int curr_row = row + blur_row;
                    int curr_col = col + blur_col;

                    if (curr_row > -1 && curr_row < height && curr_col > -1 && curr_col < data->width) {
                        int curr_index = curr_row * width + curr_col;
                        avg_red += in[curr_index].red;
                        avg_green += in[curr_index].green;
                        avg_blue += in[curr_index].blue;
                        pixel_count++;
                    }
                }
            }

            pixel_t result = {.red = (rgb_t)lroundf(avg_red / pixel_count),
                              .green = (rgb_t)lroundf(avg_green / pixel_count),
                              .blue = (rgb_t)lroundf(avg_blue / pixel_count)
                             };
            out[i] = result;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 6) {
        printf("Usage: %s <infile> <outfile> <width> <height> <num-threads\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *in = argv[1];
    char *out = argv[2];
    int width = strtol(argv[3], NULL, 10),
        height = strtol(argv[4], NULL, 10);
    long total_pixels = width * height;
    thread_count = strtol(argv[5], NULL, 10);

    // Allocate memory for images
    ppm_t *input  = (ppm_t*)malloc(sizeof(ppm_t) + (total_pixels * sizeof(pixel_t)));
    ppm_t *output = (ppm_t*)malloc(sizeof(ppm_t) + (total_pixels * sizeof(pixel_t)));

    // Read image
    START_TIMER(read)
    read_in_ppm(in, input);
    STOP_TIMER(read)


    pthread_t* threads;
    threads = malloc (thread_count*sizeof(pthread_t));
    // Verify dimensions
    if(width != input->width || height != input->height) {
        printf("ERROR: given dimensions do not match file\n");
        exit(EXIT_FAILURE);
    }

    // Copy header to output image
    copy_header_ppm(input, output);

    // Convert to greyscale
    struct ThreadData thread_data[thread_count];
    START_TIMER(grey)
    for (int i = 0; i < thread_count; i++) {
        thread_data[i].in = input->pixels;
        thread_data[i].out = output->pixels;
        thread_data[i].width = width;
        thread_data[i].height = height;
        thread_data[i].thread_id = i;
        thread_data[i].thread_count = thread_count;

        // Create thread
        if (pthread_create(&threads[i], NULL, (void* (*)(void*))color_to_grey, &thread_data[i])) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for(int thread = 0; thread < thread_count; thread++){
	    pthread_join(threads[thread], NULL);
    }
    STOP_TIMER(grey)

    // Swap buffers in preparation for blurring
    memcpy(input->pixels, output->pixels, total_pixels * sizeof(pixel_t));

    // Apply blur filter

    struct ThreadData blur_thread_data[thread_count];
    START_TIMER(blur)
    for (int i = 0; i < thread_count; i++) {
        blur_thread_data[i].in = input->pixels;
        blur_thread_data[i].out = output->pixels;
        blur_thread_data[i].width = width;
        blur_thread_data[i].height = height;
        blur_thread_data[i].thread_id = i;
        blur_thread_data[i].thread_count = thread_count;

        if (pthread_create(&threads[i], NULL, (void* (*)(void*))blur, &blur_thread_data[i])) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
    STOP_TIMER(blur)

    // Save output image
    START_TIMER(save)
    write_out_ppm(out, output);
    STOP_TIMER(save)

    // Display timing results
    printf("READ: %.6f  GREY: %.6f  BLUR: %.6f  SAVE: %.6f\n",
           GET_TIMER(read), GET_TIMER(grey), GET_TIMER(blur), GET_TIMER(save));

    free(input);
    free(output);

    return 0;
}
