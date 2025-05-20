//
// Created by Thomas on 3/21/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp8.h"

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
    fread(img->header, sizeof(unsigned char), 54, fptr);
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
    if (fptr == 0) {
        printf("impossible to create %s\n", filename);
        return;
    }
    fwrite(img->header, sizeof(unsigned char), 54, fptr);
    fwrite(img->colorTable, sizeof(unsigned char), 1024, fptr);
    fwrite(img->data, sizeof(unsigned char), img->dataSize, fptr);
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







