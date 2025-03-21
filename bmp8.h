//
// Created by Thomas on 3/21/2025.
//

#ifndef BMP8_H
#define BMP8_H
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
