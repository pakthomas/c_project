#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#include "bmp24.c"
#include "bmp8.c"


int main(){
    int choice;
    char filename[256];
    char output[256];
    t_bmp8 *img8 = NULL;
    t_bmp24 *img24 = NULL;

    do {
        printf("\n========== BMP IMAGE PROCESSING ==========\n");
        printf("1. Load 8-bit BMP Image\n");
        printf("2. Load 24-bit BMP Image\n");
        printf("3. Show Image Info (8-bit only)\n");
        printf("4. Save Image\n");
        printf("5. Apply Negative\n");
        printf("6. Adjust Brightness\n");
        printf("7. Threshold (8-bit only)\n");
        printf("8. Equalize Histogram (8-bit only)\n");
        printf("9. Convert 24-bit to Grayscale\n");
        printf("10. Exit\n");
        printf("Select an option: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1:
                printf("Enter the filename: ");
                fgets(filename, sizeof(filename), stdin);
                filename[strcspn(filename, "\n")] = 0;
                img8 = bmp8_loadImage(filename);
                if (img8) printf("Image loaded successfully.\n");
                break;

            case 2:
                printf("Enter the filename: ");
                fgets(filename, sizeof(filename), stdin);
                filename[strcspn(filename, "\n")] = 0;
                img24 = bmp24_loadImage(filename);
                if (img24) printf("Image loaded successfully.\n");
                break;

            case 3:
                if (img8) bmp8_printInfo(img8);
                else printf("No 8-bit image loaded.\n");
                break;

            case 4:
                printf("Enter output filename: ");
                fgets(output, sizeof(output), stdin);
                output[strcspn(output, "\n")] = 0;
                if (img8)
                    bmp8_saveImage(output, img8);
                else if (img24)
                    bmp24_saveImage(img24, output);
                else
                    printf("No image loaded.\n");
                break;

            case 5:
                if (img8) bmp8_negative(img8);
                else if (img24) bmp24_negative(img24);
                else printf("No image loaded.\n");
                break;

            case 6: {
                int value;
                printf("Enter brightness value (-255 to 255): ");
                scanf("%d", &value);
                if (img8) bmp8_brightness(img8, value);
                else if (img24) bmp24_brightness(img24, value);
                else printf("No image loaded.\n");
                break;
            }

            case 7: {
                if (!img8) {
                    printf("No 8-bit image loaded.\n");
                    break;
                }
                int threshold;
                printf("Enter threshold value (0 to 255): ");
                scanf("%d", &threshold);
                bmp8_threshold(img8, threshold);
                break;
            }

            case 8:
                if (img8)
                    bmp8_equalizeHistogram(img8);
                else
                    printf("No 8-bit image loaded.\n");
                break;

            case 9:
                if (img24)
                    bmp24_grayscale(img24);
                else
                    printf("No 24-bit image loaded.\n");
                break;

            case 10:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid option. Try again.\n");
        }

    } while (choice != 10);

    if (img8) bmp8_free(img8);
    // bmp24_free not implemented, add if needed
    return 0;
}


