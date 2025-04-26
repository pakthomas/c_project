//
// Created by Thomas on 3/21/2025.
//
#include <stdio.h>
#include "bmp8.h"
t_bmp8 * bmp8_loadImage(const char * filename) {
    FILE* fptr;
    fptr = fopen(filename, "r");

    if (fptr == NULL) {
        printf("The file is not opened. The program will "
               "now exit.");
        return 0;
    }
    unsigned char header[54];
    // We know that the image width is stored at offset 18 of the header
    unsigned int width = *(unsigned int *)&header[18];
    unsigned int width = *(unsigned int *)(header + 18);


    return 0;
}
