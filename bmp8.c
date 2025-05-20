//
// Created by Thomas on 3/21/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp8.h"
#include <math.h>
// Offsets for the BMP header
#define BITMAP_MAGIC 0x00 // offset 0
#define BITMAP_SIZE 0x02 // offset 2
#define BITMAP_OFFSET 0x0A // offset 10
#define BITMAP_WIDTH 0x12 // offset 18
#define BITMAP_HEIGHT 0x16 // offset 22
#define BITMAP_DEPTH 0x1C // offset 28
#define BITMAP_SIZE_RAW 0x22 // offset 34
// Magical number for BMP files
#define BMP_TYPE 0x4D42 // 'BM' in hexadecimal
// Header sizes
#define HEADER_SIZE 0x0E // 14 octets
#define INFO_SIZE 0x28 // 40 octets
// Constant for the color depth
#define DEFAULT_DEPTH 0x18 // 24
//creation the loadImage function
t_bmp8 * bmp8_loadImage(const char * filename) {
    FILE* fptr;
    fptr = fopen(filename, "r");

    if (fptr == NULL) {
        printf("The file is not opened. The program will now exit.");
        return 0;
    }
    //we create the structure img in order to save the information
    t_bmp8 *img = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (!img) {
        printf("fail of the memory location\n");
        fclose(fptr);
        return 0;
    }

    //we read the information with the fread function
    fread(img->header, sizeof(unsigned char), 54, fptr); //using an unsigned allow the variables to take positive values.
    fread(img->colorTable, sizeof(unsigned char), 1024, fptr);

    //we extract the information from the header
    img->width = *(unsigned int *)&img->header[18]; //Accessing image width (because offset = 18)
    img->height = *(unsigned int *)&img->header[22]; //Accessing image height (because offset = 22)
    img->colorDepth = *(unsigned int *)&img->header[28];//Accessing color depth (because offset = 28)
    img->dataSize = *(unsigned int *)&img->header[34];// Accessing dataSize (bcs offset = 34)

    //we verify that the img is in 8bytes
    if (img->colorDepth != 8) {
        printf("The image is not in 8 bytes.\n");
        fclose(fptr);
        free(img);
        return 0;
    }
    //we allocate memory for the data of the img
    img->data = (unsigned char *)malloc(img->dataSize);
    //we read the data of the img
    fread(img->data, sizeof(unsigned char), img->dataSize, fptr);
    fclose(fptr);
    return img;
}

//creation of the void function
void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    FILE *fptr = fopen(filename, "wb");
    if (!fptr) {
        printf("Impossible %s\n", filename);
        return;
    }

    fwrite(img->header, sizeof(char), 54, fptr);
    fwrite(img->colorTable, sizeof(char), 1024, fptr);
    fwrite(img->data, sizeof(char), img->dataSize, fptr);
    fclose(fptr);
}
// creation of the free function to free the data in the structure
void bmp8_free(t_bmp8 *img) {
    if (img != 0) {
        if (img->data != 0) {
            free(img->data);
        }
        free(img);
    }
}
//creation of the print info function
void bmp8_printInfo(t_bmp8 *img) {
    printf("Image Info:\n");
    printf("Width: %u\n", img->width);
    printf("Height: %u\n", img->height);
    printf("Color Depth: %u\n", img->colorDepth);
    printf("Data Size: %u\n", img->dataSize);
}
// creation of the negative function
void bmp8_negative(t_bmp8 *img) {
    for (unsigned int i = 0; i < img->dataSize; i++) {
        //for each pixel we substract 255
        img->data[i] = 255 - img->data[i];
    }
}

//creation of the brightness function
// if the value of the pixel is >255 it is set as 255 if its bellow 0 it is set as 0.
void bmp8_brightness(t_bmp8 *img, int value) {
    for (unsigned int i = 0; i < img->dataSize; i++) {
        int temp = img->data[i] + value;
        if (temp > 255) temp = 255;
        if (temp < 0) temp = 0;
        img->data[i] = (unsigned char)temp;
    }
}


//creation of the threshold function; to creat a balck and white img
// if the value of the pixel>threshold the pixel is set to 255, if it is bellow it is set to 0
void bmp8_threshold(t_bmp8 *img, int threshold) {
    for (unsigned int i = 0; i< img->dataSize; i++) {
        if (img->data[i] >= threshold)
            img->data[i] = 255;
        else
            img->data[i] = 0;
    }
}


// creation of the apply filter function
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    int n = kernelSize / 2;
    unsigned char *newData = (unsigned char *)malloc(img->dataSize);

    memcpy(newData, img->data, img->dataSize);

    for (unsigned int y = 1;y < img->height-1; y++) {
        for (unsigned int x = 1; x < img->width -1;x++) {
            float sum = 0.0f;
            for (int j = -n; j <= n; j++) {
                for (int i = -n; i <= n; i++) {
                    int pixel = img->data[(y+j) * img->width + (x-i)];
                    sum += pixel * kernel[j + n][i + n];
                }
            }
            if (sum > 255) sum = 255;
            if (sum < 0) sum = 0;
            newData[y * img->width + x] = (unsigned char)sum;
        }
    }
    free(img->data);
    img->data = newData;
}
t_bmp24 * bmp24_allocate (int width, int height, int colorDepth) {
    t_bmp24 * img = (t_bmp24 *)malloc(sizeof(t_bmp24));
    return img;
}
void file_rawRead (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
fseek(file, position, SEEK_SET);
fread(buffer, size, n, file);
}

void file_rawWrite (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}

void bmp24_readPixelData(t_bmp24 *image, FILE *file) {
    // Get the offset to the pixel data (stored at byte 10 in the header)
    uint32_t offset = *(uint32_t *)(((uint8_t *)&image->header) + BITMAP_OFFSET);
    fseek(file, offset, SEEK_SET);

    int width = image->width;
    int height = image->height;

    for (int y = height - 1; y >= 0; y--) { // BMP rows start from bottom to top and from left to right -_-
        for (int x = 0; x < width; x++) {
            uint8_t rgb[3];
            if (fread(rgb, sizeof(uint8_t), 3, file) != 3) {
                fprintf(stderr, "Error reading pixel at (%d, %d)\n", x, y);
                return;
            }
            //store the RGB values of each pixel:
            image->data[y][x].red   = rgb[0];
            image->data[y][x].green = rgb[1];
            image->data[y][x].blue  = rgb[2];
        }
    }
}

void bmp24_writePixelData (t_bmp24 * image, FILE * file) {
    for (int y = image->height - 1; y >= 0; y--) {
        for (int x = 0; x < image->width; x++) {
            uint8_t rgb[3];
            rgb[0] = image->data[y][x].red;
            rgb[1] = image->data[y][x].green;
            rgb[2] = image->data[y][x].blue;
            fwrite(rgb, sizeof(uint8_t), 3, file); // stores the rgb composition of the pixel in the file
                return;
        }
    }
}




t_bmp24 * bmp24_loadImage (const char * filename){
    FILE * fptr;
    t_bmp24 * img  = malloc(sizeof(t_bmp24));
    fptr =fopen(filename,"rb");
    if (fptr == NULL) {
        printf("Error! The program will now exit.");
        return 0;
    }
    t_bmp_header header;
    t_bmp24 header_info;
    file_rawRead(BITMAP_MAGIC, &header, sizeof(t_bmp_header), 1, fptr);
    file_rawRead(BITMAP_SIZE, &header_info, sizeof(t_bmp_header), 1, fptr);
    file_rawWrite(BITMAP_MAGIC, &header, sizeof(t_bmp_header), 1, img);
    file_rawWrite(BITMAP_SIZE, &header_info, sizeof(t_bmp_header), 1, img);
    bmp24_readPixelData(img, fptr);
    fclose(fptr);
    return img;
}

void bmp24_saveImage (t_bmp24 * img, const char * filename) {
    FILE * fptr;
    fptr =fopen(filename,"rb");
    t_bmp_header header;
    if (fptr == NULL) {
        printf("Error! The program will now exit.");
        return;
    }
    file_rawWrite( BITMAP_MAGIC,&header, HEADER_SIZE, 1, fptr);
    bmp24_writePixelData(img,fptr);
    fclose(fptr);
}

void bmp24_negative(t_bmp24 *img) {
    for (unsigned int i = 0; i < img->height; i++) {
        for (unsigned int j = 0; j < img->width; j++) {
            img->data[i][j].red = 255 - img->data[i][j].red;
            img->data[i][j].green = 255 - img->data[i][j].green;
            img->data[i][j].blue = 255 - img->data[i][j].blue;
        }
    }
}

void bmp24_grayscale (t_bmp24 * img) {
    for (unsigned int i = 0; i < img->height; i++) {
        for (unsigned int j = 0; j < img->width; j++) {
            int average = (img->data[i][j].red+img->data[i][j].green+img->data[i][j].blue)/3;
            img->data[i][j].red = average;
            img->data[i][j].green = average;
            img->data[i][j].blue = average;
        }
    }
}

void bmp24_brightness (t_bmp24 * img,int value) {
    for (unsigned int i = 0; i < img->height; i++) {
        for (unsigned int j = 0; j < img->width; j++) {
            if (img->data[i][j].red + value <= 255)
            img->data[i][j].red = (img->data[i][j].red+value);
            else img->data[i][j].red = 255;
            if (img->data[i][j].green + value <= 255)
            img->data[i][j].green = (img->data[i][j].green+value);
            else img->data[i][j].green = 255;
            if (img->data[i][j].blue + value <= 255)
            img->data[i][j].blue = (img->data[i][j].blue+value);
            else img->data[i][j].blue = 255;
        }
    }
}
//creation of the compute histogram function, the function goes through each function and store its value in a list => the aim of this funcction is to count how many pixels there is for each gray color in the file
void bmp8_computeHistogram(t_bmp8 *img, int histogram[256]) {
    // initialise all possible value of the pixels to zero
    for (int i = 0; i < 256; i++) {
        histogram[i] = 0;
    }

    // go through every pixel and add +1 to the value of the color of the pixel
    for (unsigned int i = 0; i < img->dataSize; i++) {
        int gray = img->data[i];
        histogram[gray]++;
    }
}
void bmp8_equalizeHistogram(t_bmp8 *img) {
    int histogram[256] = {0};

    // step 1 : Calculate the histogram of the original image
    for (unsigned int i = 0; i < img->dataSize; i++) {
        histogram[img->data[i]]++;
    }

    // step 2 : Calculate the cumulative histogram using a CDF
    int cdf[256];
    cdf[0] = histogram[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + histogram[i];
    }

    // step 3 :Normalize the CDF to obtain a new scale of gray levels.
    int totalPixels = img->width * img->height;
    unsigned char mapping[256];
    for (int i = 0; i < 256; i++) {
        mapping[i] = (unsigned char)((cdf[i] - cdf[0]) * 255 / (totalPixels - cdf[0]));
    }

    // step 4 : Apply the transformation to each pixel of the original image using the new scale of gray levels.
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = mapping[img->data[i]];
    }
}

int * bmp8_computeHistogram(t_bmp8 *img) {
    // Allocate memory for the histogram (256 gray levels, initialized to 0)
    int *hist = (int *)calloc(256, sizeof(int));
    if (hist == NULL) {
        printf("Error: could not allocate memory for histogram\n");
        return NULL;
    }

    // Loop over all pixels in the image data
    for (int i = 0; i < img->dataSize; i++) {
        // Get the pixel value (0-255)
        int pixel = (unsigned char)img->data[i];
        // Increase the count for that pixel value in the histogram
        hist[pixel]++;
    }

    // Return the histogram array (int[256])
    return hist;
}



int * bmp8_computeCDF(int *hist) {
    // Allocate memory for the cumulative histogram (CDF)
    int *cdf = (int *)calloc(256, sizeof(int));
    if (cdf == NULL) {
        printf("Error: could not allocate memory for CDF\n");
        return NULL;
    }

    // Step 1: Compute the cumulative sum of the histogram
    cdf[0] = hist[0]; // first value is same
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + hist[i]; // cdf[i] = cdf[i-1] + hist[i]
    }

    // Step 2: Find the minimum non-zero value in the CDF (cdfmin)
    int cdfmin = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] != 0) {
            cdfmin = cdf[i];
            break; // found it
        }
    }

    // Step 3: Normalize the CDF using the given formula:
    // hist_eq[i] = round((cdf[i] - cdfmin) / (N - cdfmin) * 255)
    int N = cdf[255]; // total number of pixels in image

    // Allocate memory for the equalized histogram (normalized CDF)
    int *hist_eq = (int *)calloc(256, sizeof(int));
    if (hist_eq == NULL) {
        printf("Error: could not allocate memory for hist_eq\n");
        free(cdf);
        return NULL;
    }

    // Apply the normalization formula to each gray level
    for (int i = 0; i < 256; i++) {
        hist_eq[i] = (int)round(((double)(cdf[i] - cdfmin) / (N - cdfmin)) * 255);
    }

    // We don't need the raw CDF anymore, so free it
    free(cdf);

    // Return the normalized histogram (used for equalization)
    return hist_eq;
}








