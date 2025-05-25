/**
*  main.c
 * Thomas PAK and Laure SZYMANSKI
 * Main file for loading, saving, filtering, and displaying BMP images in both 8-bit (grayscale)
 *        and 24-bit (color) formats. Provides a command-line menu interface for user interaction.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#include "bmp24.c"
#include "bmp8.c"



void choose_filter(void * img, int value); // Forward declaration for filter menu

/**
 *  Main interactive loop that allows users to:
 *        - Open a BMP image
 *        - Save a BMP image
 *        - Apply filters
 *        - Display image info
 *        - Exit the program
 */
int main() {
    int choice;
    char file_name[100];
    int is_color = -1; // Image type flag: -1 = no image loaded, 0 = grayscale, 1 = color
    t_bmp8 * img8 = NULL;
    t_bmp24 * img24 = NULL;

    while (1) {
        // Display main menu
        printf("Please choose an option:\n");
        printf("1. Open an image\n");
        printf("2. Save an image\n");
        printf("3. Apply a filter\n");
        printf("4. Display image information\n");
        printf("5. Quit\n");
        printf(">>> Your choice: ");
        scanf("%d", &choice);
        getchar(); // no take off the newline character

        switch (choice) {
            case 1: {
                // Load image (grayscale or color)
                while (is_color < 0 || is_color > 1) {
                    printf("Is the image:\n"
                           "0. Grayscale (8-bit)\n"
                           "1. Color (24-bit)\n>>> ");
                    scanf("%d", &is_color);
                }

                printf("Enter the name of the file: ");
                scanf("%s", file_name);
                getchar();

                if (is_color == 1) {
                    img24 = bmp24_loadImage(file_name);
                    if (img24 != NULL) printf("Color image loaded successfully!\n");
                    else printf("Failed to load color image.\n");
                } else if (is_color == 0) {
                    img8 = bmp8_loadImage(file_name);
                    if (img8 != NULL) printf("Grayscale image loaded successfully!\n");
                    else printf("Failed to load grayscale image.\n");
                } else {
                    printf("Invalid selection.\n");
                    is_color = -1;
                }
                break;
            }
            case 2: {
                // Save image
                if (is_color == 1 && img24 != NULL) {
                    printf("Enter output file name: ");
                    scanf("%s", file_name);
                    bmp24_saveImage(img24, file_name);
                    printf("Color image saved successfully!\n");
                } else if (is_color == 0 && img8 != NULL) {
                    printf("Enter output file name: ");
                    scanf("%s", file_name);
                    bmp8_saveImage(file_name, img8);
                    printf("Grayscale image saved successfully!\n");
                } else {
                    printf("No image loaded.\n");
                }
                break;
            }
            case 3: {
                // Apply selected filter
                if (is_color == 0 && img8 != NULL) {
                    choose_filter(img8, is_color); // Appelle le sous-menu des filtres pour img8
                } else if (is_color == 1 && img24 != NULL) {
                    choose_filter(img24, is_color); // Appelle le sous-menu des filtres pour img24
                } else {
                    printf("No image loaded.\n");
                }
                break;
            }
            case 4: {
                // Display image information
                if (is_color == 1 && img24 != NULL) {
                    bmp24_printInfo(img24);
                } else if (is_color == 0 && img8 != NULL) {
                    bmp8_printInfo(img8);
                } else {
                    printf("No image loaded.\n");
                }
                break;
            }
            case 5: {
                // Exit the program
                printf("Goodbye!\n");
                return 0;
            }
            default: {
                printf("Invalid choice. Please try again.\n");
            }
        }
    }
}

float** box_blur_kernel() {
    float kernel_values[3][3] = {
        {1/9.0f, 1/9.0f, 1/9.0f},
        {1/9.0f, 1/9.0f, 1/9.0f},
        {1/9.0f, 1/9.0f, 1/9.0f}
    };
    float** kernel = malloc(3 * sizeof(float*));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = kernel_values[i][j];
        }
    }
    return kernel;
}

float** gaussian_blur_kernel() {
    float kernel_values[3][3] = {
        {1/16.0f, 2/16.0f, 1/16.0f},
        {2/16.0f, 4/16.0f, 2/16.0f},
        {1/16.0f, 2/16.0f, 1/16.0f}
    };
    float** kernel = malloc(3 * sizeof(float*));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = kernel_values[i][j];
        }
    }
    return kernel;
}

float** sharpen_kernel() {
    float kernel_values[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };
    float** kernel = malloc(3 * sizeof(float*));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = kernel_values[i][j];
        }
    }
    return kernel;
}

float** outline_kernel() {
    float kernel_values[3][3] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };
    float** kernel = malloc(3 * sizeof(float*));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = kernel_values[i][j];
        }
    }
    return kernel;
}

float** emboss_kernel() {
    float kernel_values[3][3] = {
        {-2, -1,  0},
        {-1,  1,  1},
        { 0,  1,  2}
    };
    float** kernel = malloc(3 * sizeof(float*));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = kernel_values[i][j];
        }
    }
    return kernel;
}

void bmp24_boxBlur(t_bmp24* img) {
    float** kernel = box_blur_kernel();
    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            img->data[y][x] = bmp24_convolution(img, x, y, kernel, 3);
        }
    }
    // Free kernel memory
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_gaussianBlur(t_bmp24* img) {
    float** kernel = gaussian_blur_kernel();
    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            img->data[y][x] = bmp24_convolution(img, x, y, kernel, 3);
        }
    }
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_sharpen(t_bmp24* img) {
    float** kernel = sharpen_kernel();
    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            img->data[y][x] = bmp24_convolution(img, x, y, kernel, 3);
        }
    }
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_outline(t_bmp24* img) {
    float** kernel = outline_kernel();
    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            img->data[y][x] = bmp24_convolution(img, x, y, kernel, 3);
        }
    }
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_emboss(t_bmp24* img) {
    float** kernel = emboss_kernel();
    for (int y = 1; y < img->height - 1; y++) {
        for (int x = 1; x < img->width - 1; x++) {
            img->data[y][x] = bmp24_convolution(img, x, y, kernel, 3);
        }
    }
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}

/**
 *  Displays filter menu and applies selected filter to the image
 *
 * the parameter image is a void pointer to the image (either t_bmp8 or t_bmp24)
 * the parameter type is g0 for grayscale, 1 for color
 */
void choose_filter(void* img, int type) {
    int filter_choice;

    while (1) {
        printf("Please choose a filter:\n");
        printf("1. Negative\n");
        printf("2. Brightness\n");
        printf("3. Black and white (threshold / grayscale)\n");
        printf("4. Box Blur\n");
        printf("5. Gaussian blur\n");
        printf("6. Sharpness\n");
        printf("7. Outline\n");
        printf("8. Emboss\n");
        printf("9. Return to the previous menu\n");
        printf(">>> Your choice: ");
        scanf("%d", &filter_choice);
        getchar();

        if (type == 0) { // Grayscale image (bmp8)
            t_bmp8* img8 = (t_bmp8*)img;
            switch (filter_choice) {
                case 1:
                    bmp8_negative(img8);
                    break;
                case 2: {
                    int brightness;
                    printf("Enter brightness level (-255 to 255): ");
                    scanf("%d", &brightness);
                    getchar();
                    if (brightness < -255 || brightness > 255) {
                        printf("Brightness out of range.\n");
                        break;
                    }
                    bmp8_brightness(img8, brightness);
                    break;
                }
                case 3: {
                    int threshold;
                    printf("Enter threshold value (0–255): ");
                    scanf("%d", &threshold);
                    getchar();
                    if (threshold < 0 || threshold > 255) {
                        printf("Threshold must be between 0 and 255.\n");
                        break;
                    }
                    bmp8_threshold(img8, threshold);
                    break;
                }
                case 4:
                    bmp8_applyFilter(img8, (float**)box_blur_kernel(), 3);
                    break;
                case 5:
                    bmp8_applyFilter(img8, (float**)gaussian_blur_kernel(), 3);
                    break;
                case 6:
                    bmp8_applyFilter(img8, (float**)sharpen_kernel(), 3);
                    break;
                case 7:
                    bmp8_applyFilter(img8, (float**)outline_kernel(), 3);
                    break;
                case 8:
                    bmp8_applyFilter(img8, (float**)emboss_kernel(), 3);
                    break;
                case 9:
                    return;
                default:
                    printf("Invalid choice. Please try again.\n");
            }
            printf("Filter applied successfully!\n\n");

        } else if (type == 1) { // Color image (bmp24)
            t_bmp24* img24 = (t_bmp24*)img;
            switch (filter_choice) {
                case 1:
                    bmp24_negative(img24);
                    break;
                case 2: {
                    int brightness;
                    printf("Enter brightness level (-255 to 255): ");
                    scanf("%d", &brightness);
                    getchar();
                    if (brightness < -255 || brightness > 255) {
                        printf("Brightness out of range.\n");
                        break;
                    }
                    bmp24_brightness(img24, brightness);
                    break;
                }
                case 3:
                    bmp24_grayscale(img24);
                    break;
                case 4:
                    bmp24_boxBlur(img24);
                    break;
                case 5:
                    bmp24_gaussianBlur(img24);
                    break;
                case 6:
                    bmp24_sharpen(img24);
                    break;
                case 7:
                    bmp24_outline(img24);
                    break;
                case 8:
                    bmp24_emboss(img24);
                    break;
                case 9:
                    return;
                default:
                    printf("Invalid choice. Please try again.\n");
            }
            printf("Filter applied successfully!\n\n");

        } else {
            printf("No image loaded or unknown image type.\n");
            return;
        }
    }
}
