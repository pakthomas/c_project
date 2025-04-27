//
// Created by Thomas on 3/21/2025.
//

#ifndef BMP8_H
#define BMP8_H
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

//definition of the functions

t_bmp8 * bmp8_loadImage(const char * filename);
void bmp8_saveImage(const char * filename, t_bmp8 * img);
void bmp8_free(t_bmp8 * img);
void bmp8_printInfo(t_bmp8 * img);

//definition of the treatment function
void bmp8_negative(t_bmp8 *img);
void bmp8_brightness(t_bmp8 *img, int value);
void bmp8_threshold(t_bmp8 *img, int threshold);

//definition of the apllyfilter function
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize);

#endif