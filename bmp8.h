//
// Created by Thomas on 3/21/2025.
//

#ifndef BMP8_H
#define BMP8_H

#include <stdint.h>

//definition of a structured type t_bmp8
typedef struct {
    unsigned char header[54];
    unsigned char colorTable[1024];
    unsigned char * data;
    unsigned int width;
    unsigned int height;
    unsigned int colorDepth;
    unsigned int dataSize;
} t_bmp8;
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} t_bmp_header;
typedef struct {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits;
    uint32_t compression;
    uint32_t imagesize;
    int32_t xresolution;
    int32_t yresolution;
    uint32_t ncolors;
    uint32_t importantcolors;
}t_bmp_info;
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_pixel;


//definition of the functions

t_bmp8 * bmp8_loadImage(const char * filename);
void bmp8_saveImage(const char * filename, t_bmp8 * img);
void bmp8_free(t_bmp8 * img);
void bmp8_printInfo(t_bmp8 * img);

//definition of the treatment functions
void bmp8_negative(t_bmp8 *img);
void bmp8_brightness(t_bmp8 *img, int value);
void bmp8_threshold(t_bmp8 *img, int threshold);

//definition of the apllyfilter functions
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize);


#endif
