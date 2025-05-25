#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#include "bmp24.c"
#include "bmp8.c"


void choose_filter(void * img, int value); // Déclaration préalable

int main() {
    int choice;
    char file_name[100];
    int is_color = -1; // 0 = noir et blanc, 1 = couleur, -1 = aucune image chargée
    t_bmp8 * img8 = NULL;
    t_bmp24 * img24 = NULL;

    while (1) {
        printf("Please choose an option:\n");
        printf("1. Open an image\n");
        printf("2. Save an image\n");
        printf("3. Apply a filter\n");
        printf("4. Display image information\n");
        printf("5. Quit\n");
        printf(">>> Your choice: ");
        scanf("%d", &choice);
        getchar(); // pour consommer le \n

        switch (choice) {
            case 1: {
                printf("Is the image:\n"
                       "0. Grayscale (8-bit)\n"
                       "1. Color (24-bit)\n>>> ");
                scanf("%d", &is_color);
                getchar();

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
                if (is_color == 1 && img24 != NULL) {
                    printf("Enter output file name: ");
                    scanf("%s", file_name);
                    bmp24_saveImage(file_name, img24);
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
                if (is_color == 0 && img8 != NULL) {
                    choose_filter(img8, is_color); // Appelle le sous-menu des filtres pour img8
                } else if (is_color == 1 && img24 != NULL) {
                    printf("Filter menu for color images is not implemented yet.\n");
                } else {
                    printf("No image loaded.\n");
                }
                break;
            }
            case 4: {
                if (is_color == 1 && img24 != NULL) {
                    // bmp24_printInfo(img24);
                } else if (is_color == 0 && img8 != NULL) {
                    bmp8_printInfo(img8);
                } else {
                    printf("No image loaded.\n");
                }
                break;
            }
            case 5: {
                printf("Goodbye!\n");
                return 0;
            }
            default: {
                printf("Invalid choice. Please try again.\n");
            }
        }
    }
}

void choose_filter(void* img, int type) {
    int filter_choice;

    while (1) {
        printf("Please choose a filter:\n");
        printf("1. Negative\n");
        printf("2. Brightness\n");
        printf("3. Black and white\n");
        printf("4. Box Blur\n");
        printf("5. Gaussian blur\n");
        printf("6. Sharpness\n");
        printf("7. Outline\n");
        printf("8. Emboss\n");
        printf("9. Return to the previous menu\n");
        printf(">>> Your choice: ");
        scanf("%d", &filter_choice);
        getchar();

        switch (filter_choice) {
            case 1:
                if (type == 0) bmp8_negative((t_bmp8*)img);
                else if (type == 1) bmp24_negative((t_bmp24*)img);
                break;

            case 2: {
                int brightness_value;
                printf("Enter the brightness level (-255 to 255): ");
                scanf("%d", &brightness_value);
                getchar();
                if (brightness_value < -255 || brightness_value > 255) {
                    printf("The brightness level is not in the valid range.\n");
                    break;
                }
                if (type == 0) bmp8_brightness((t_bmp8*)img, brightness_value);
                else if (type == 1) bmp24_brightness((t_bmp24*)img, brightness_value);
                break;
            }

            case 3:
                if (type == 0) bmp8_blackWhite((t_bmp8*)img);
                else printf("Black and white filter not available for color images.\n");
                break;

            case 4:
                if (type == 0) bmp8_boxBlur((t_bmp8*)img);
                else printf("Box blur filter not available for color images yet.\n");
                break;

            case 5:
                if (type == 0) bmp8_gaussianBlur((t_bmp8*)img);
                else printf("Gaussian blur not implemented for color images.\n");
                break;

            case 6:
                if (type == 0) bmp8_sharpen((t_bmp8*)img);
                else printf("Sharpen filter not implemented for color images.\n");
                break;

            case 7:
                if (type == 0) bmp8_outline((t_bmp8*)img);
                else printf("Outline filter not implemented for color images.\n");
                break;

            case 8:
                if (type == 0) bmp8_emboss((t_bmp8*)img);
                else printf("Emboss filter not implemented for color images.\n");
                break;

            case 9:
                return;

            default:
                printf("Invalid choice. Please try again.\n");
        }

        printf("Filter applied successfully!\n\n");
    }
}
