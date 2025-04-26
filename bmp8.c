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
    //we creat the structure img to keep the informations
    t_bmp8 *img = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (!img) {
        printf("fail of the memory location\n");
        fclose(fptr);
        return 0;
    }

    //we read the information with the fread
    fread(img->header, sizeof(unsigned char), 54, f);
    fread(img->colorTable, sizeof(unsigned char), 1024, f);

    //we extract the informations
    img->width = *(unsigned int *)&img->header[18];
    img->height = *(unsigned int *)&img->header[22];
    img->colorDepth = *(unsigned int *)&img->header[28];
    img->dataSize = *(unsigned int *)&img->header[34];

    //we verify that the img is in 8bytes
    if (img->colorDepth != 8) {
        printf("The image is not in 8 bytes.\n");
        fclose(f);
        free(img);
        return NULL;
    }
    //we allocate memory for the data of the img
    img->data = (unsigned char *)malloc(img->dataSize);
    //we read the data of the img
    fread(img->data, sizeof(unsigned char), img->dataSize, f);
    fclose(f);

    return img;
}







    return 0;
}
