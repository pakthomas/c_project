//
// Created by Thomas on 5/20/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
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


t_pixel bmp24_convolution(t_bmp24 *img, int x, int y, float **kernel, int kernelSize) {
    int offset = kernelSize / 2;
    float redSum = 0, greenSum = 0, blueSum = 0;
    for (int i = -offset; i <= offset; i++) { //In order to multiply the pixel and the ones surrounding it with the Kernel Matrix, we need to use a nested loop to traverse the whole matrix
        for (int j = -offset; j <= offset; j++) {
            int x_coordinate = x +j;
            int y_coordinate =y + i;
            t_pixel p = img->data[y_coordinate][x_coordinate];  // Get access to the elements in the pixel matrix
            float k = kernel[i + offset][j + offset];// Get access to the elements in the Kernel matrix
            redSum += p.red * k; //Multiply the pixel value by its respective kernel value, and add to the sum.
            greenSum += p.green * k;
            blueSum += p.blue * k;
        }
    }
    t_pixel new_pixel;
    new_pixel.red   = (uint8_t)fminf(fmaxf(redSum, 0), 255); //Use of fminf and fmaxf to assure the value stays between 0 and 255
    new_pixel.green = (uint8_t)fminf(fmaxf(greenSum, 0), 255);
    new_pixel.blue  = (uint8_t)fminf(fmaxf(blueSum, 0), 255);
    return new_pixel;
}
void bmp24_equalize(t_bmp24 * img) {
    int histogram[256];
    int gray;
    int U;
    int V;
    for (int i = 0; i < 256; i++) {
        histogram[i] = 0;
    }
    for (int i = 0; img->width ; i++) {
        for (int j = 0; img->height ; j++) {
            gray = 0.299*img->data[i][j].red + 0.587*img->data[i][j].green + 0.114*img->data[i][j].blue;
            U = -0.14713*img->data[i][j].red-0.28886 * img->data[i][j].green+0.436*img->data[i][j].blue;
            V = 0.615 * img->data[i][j].red - 0.51499*img->data[i][j].green - 0.10001*img->data[i][j].blue;
            histogram[gray]++;//1. Calculate the histogram of the original image.
        }
    }
    int cumul_histogram[256];
    cumul_histogram[0] = histogram[0];
    int min=histogram[0];
    int nb_pixels = 0;
    for (int i = 1; i < 256; i++) {
        cumul_histogram[i] = histogram[i]+cumul_histogram[i-1];//2. Calculate the cumulative histogram using a CDF.
        if (cumul_histogram[i] < min || min ==0) {
            min = cumul_histogram[i];
        }
    }
    int hist_eq[256];
    for (int i = 0; i < 256; i++) {
        hist_eq[i]=round((cumul_histogram[i]-min)/(nb_pixels-min) * 255);//3. Normalize the CDF to obtain a new scale of gray levels.

    }
    int Y = hist_eq[(int)round(gray)];
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            int R = img->data[i][j].red;
            int G = img->data[i][j].green;
            int B = img->data[i][j].blue;
            double Y = 0.299 * R + 0.587 * G + 0.114 * B;
            double U = -0.14713 * R - 0.28886 * G + 0.436 * B;
            double V = 0.615 * R - 0.51499 * G - 0.10001 * B;
            int R_1 = round(Y + 1.13983 * V);
            int G_1 = round(Y - 0.39465 * U - 0.58060 * V);
            int B_1 = round(Y + 2.03211 * U);
            img->data[i][j].red  = fminf(fmaxf(R_1, 0), 255);
            img->data[i][j].green = fminf(fmaxf(G_1, 0), 255);
            img->data[i][j].blue  = fminf(fmaxf(B_1, 0), 255);
        }
    }
}//4. Apply the transformation to each pixel of the original image using the new scale of gray levels.
