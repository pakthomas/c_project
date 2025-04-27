#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"
int main(void) {
    bmp8_loadImage("lena_gray.bmp");
    return 0;
}