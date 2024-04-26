#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 200
double calculate_percentage_difference(char *image1_path, char *image2_path) {
    char command[MAX_COMMAND_LENGTH];
    FILE *fp;
    double distortion, percentage_difference;
    int width, height, total_pixels;

    // Generate the difference image using ImageMagick
    snprintf(command, sizeof(command), "compare -metric AE -fuzz 5%% %s %s difference.png", image1_path, image2_path);
    system(command);

    // Get the output from ImageMagick's compare command
    snprintf(command, sizeof(command), "identify -format %%[distortion] difference.png");
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Error: Failed to execute command\n");
        exit(1);
    }
    fscanf(fp, "%lf", &distortion);
    pclose(fp);

    // Get the dimensions of the original image
    snprintf(command, sizeof(command), "identify -format %%wx%%h %s", image1_path);
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Error: Failed to execute command\n");
        exit(1);
    }
    fscanf(fp, "%dx%d", &width, &height);
    pclose(fp);

    total_pixels = (width * height);

    // Calculate the percentage difference
    percentage_difference = (distortion / total_pixels) * 100;

    return percentage_difference;
}

int main(int argc, char *argv[]) {
    // Check if the correct number of arguments are provided
    if (argc != 3) {
        printf("Usage: %s <image1_path> <image2_path>\n", argv[0]);
        return 1; // Return error code
    }
    char *image1_path = argv[1];
    char *image2_path = argv[2];
    double percentage_diff = calculate_percentage_difference(image1_path, image2_path);
    printf("\rPercentage difference between %s and %s: %.2f%%\n", image1_path, image2_path, percentage_diff);

    return 0;
}
