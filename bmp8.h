//
// Created by Thomas on 3/21/2025.
//

#ifndef BMP8_H
#define BMP8_H
//definition of a structured type t_bmp8 to represent an 8-bit grayscale image
typedef struct {
    unsigned char header[54];
    unsigned char colorTable[1024];
    unsigned char * data;
    unsigned int width;
    unsigned int height;
    unsigned int colorDepth;
    unsigned int dataSize;
} t_bmp8;
#endif //BMP8_H

//definition of the function
t_bmp8 * bmp8_loadImage(const char * filename);
void bmp8_saveImage(const char * filename, t_bmp8 * img);
void bmp8_free(t_bmp8 * img);
void bmp8_printInfo(t_bmp8 * img);
