//
// Created by Thomas on 5/20/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp24.h"
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

void bmp24_readPixelData(t_bmp24 *img, FILE *fptr) {
    int w = img->width;
    int h = img->height;
    int rowSize = (w * 3 + 3) & ~3;
    int padding = rowSize - w * 3;

    // Allouer la matrice
    img->data = malloc(h * sizeof(t_pixel*));
    for (int y = 0; y < h; y++) {
        img->data[y] = malloc(w * sizeof(t_pixel));
    }

    // Lire ligne par ligne, de bas en haut
    for (int y = h - 1; y >= 0; y--) {
        for (int x = 0; x < w; x++) {
            uint8_t bgr[3];
            fread(bgr, sizeof bgr, 1, fptr);
            img->data[y][x].blue  = bgr[0];
            img->data[y][x].green = bgr[1];
            img->data[y][x].red   = bgr[2];
        }
        fseek(fptr, padding, SEEK_CUR);
    }
}



void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    int padding = (4 - (image->width * 3) % 4) % 4; // BMP row padding to 4 bytes
    uint8_t pad[3] = {0, 0, 0};

    for (int y = image->height - 1; y >= 0; y--) {
        for (int x = 0; x < image->width; x++) {
            uint8_t rgb[3];
            rgb[2] = image->data[y][x].red;
            rgb[1] = image->data[y][x].green;
            rgb[0] = image->data[y][x].blue;
            fwrite(rgb, sizeof(uint8_t), 3, file);
        }
        // Écriture du padding (alignement sur 4 octets)
        fwrite(pad, sizeof(uint8_t), padding, file);
    }
}
t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *fptr = fopen(filename, "rb");
    if (!fptr) {
        printf("Error! Cannot open %s\n", filename);
        return NULL;
    }

    // 1) Lire les 54 octets de header brut
    uint8_t raw[54];
    if (fread(raw, 1, 54, fptr) != 54) {
        printf("Error! Cannot read BMP header.\n");
        fclose(fptr);
        return NULL;
    }

    // 2) Allouer la structure
    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (!img) {
        printf("Error! Memory allocation failed.\n");
        fclose(fptr);
        return NULL;
    }

    // 3) Décoder le file header
    img->header.type      = *(uint16_t *)(raw + 0);
    img->header.size      = *(uint32_t *)(raw + 2);
    img->header.reserved1 = *(uint16_t *)(raw + 6);
    img->header.reserved2 = *(uint16_t *)(raw + 8);
    img->header.offset    = *(uint32_t *)(raw + 10);

    // 4) Décoder le info header
    img->header_info.size           = *(uint32_t *)(raw + 14);
    img->header_info.width          = *(int32_t  *)(raw + 18);
    img->header_info.height         = *(int32_t  *)(raw + 22);
    img->header_info.planes         = *(uint16_t *)(raw + 26);
    img->header_info.bits           = *(uint16_t *)(raw + 28);
    img->header_info.compression    = *(uint32_t *)(raw + 30);
    img->header_info.imagesize      = *(uint32_t *)(raw + 34);
    img->header_info.xresolution    = *(int32_t  *)(raw + 38);
    img->header_info.yresolution    = *(int32_t  *)(raw + 42);
    img->header_info.ncolors        = *(uint32_t *)(raw + 46);
    img->header_info.importantcolors= *(uint32_t *)(raw + 50);

    // 5) Initialiser width/height/depth
    img->width      = img->header_info.width;
    img->height     = img->header_info.height;
    img->colorDepth = img->header_info.bits;

    // 6) Sanity checks
    if (img->width <= 0 || img->height <= 0 || img->colorDepth != 24) {
        printf("Unsupported BMP: %d×%d @ %d bits\n",
               img->width, img->height, img->colorDepth);
        free(img);
        fclose(fptr);
        return NULL;
    }

    // 7) Se placer au début des pixels
    fseek(fptr, img->header.offset, SEEK_SET);

    // 8) Lire les données pixel
    bmp24_readPixelData(img, fptr);

    fclose(fptr);
    return img;
}


void bmp24_printInfo(t_bmp24 *img) {
    if (img == NULL) {
        printf("No image loaded.\n");
        return;
    }

    printf("📄 BMP 24-bit Image Information:\n");
    printf(" - Width         : %d px\n", img->header_info.width);
    printf(" - Height        : %d px\n", img->header_info.height);
    printf(" - Bit depth     : %d bits/pixel\n", img->header_info.bits);
    printf(" - Image size    : %u bytes\n", img->header_info.size);
    printf(" - File size     : %u bytes\n", img->header.size);
    printf(" - Pixel offset  : %u bytes\n", img->header.offset);
    printf(" - Compression   : %u\n", img->header_info.compression);
    printf(" - Colors used   : %u\n", img->header_info.ncolors);
}

// Écris headers + pixels
void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        printf("Cannot create %s\n", filename);
        return;
    }

    int w = img->width, h = img->height;
    int rowSize       = (w*3 + 3) & ~3;    // aligné sur 4
    int pixelDataSize = rowSize * h;
    int fileSize      = 14 + 40 + pixelDataSize;

    // --- FILE HEADER (14 octets) ---
    uint8_t header[14] = {
        'B','M',                             // magic
        (uint8_t)(fileSize      &0xFF),     // size little-endian
        (uint8_t)((fileSize>> 8)&0xFF),
        (uint8_t)((fileSize>>16)&0xFF),
        (uint8_t)((fileSize>>24)&0xFF),
        0,0, 0,0,                            // reserved
        54,0,0,0                            // offset = 14+40 = 54
    };
    fwrite(header, 1, 14, f);

    // --- INFO HEADER (40 octets) ---
    uint8_t info[40] = {0};
    // size of this header
    info[0] = 40;
    // width
    info[4] = (uint8_t)(w      &0xFF);
    info[5] = (uint8_t)((w>> 8)&0xFF);
    info[6] = (uint8_t)((w>>16)&0xFF);
    info[7] = (uint8_t)((w>>24)&0xFF);
    // height
    info[8]  = (uint8_t)(h      &0xFF);
    info[9]  = (uint8_t)((h>> 8)&0xFF);
    info[10] = (uint8_t)((h>>16)&0xFF);
    info[11] = (uint8_t)((h>>24)&0xFF);
    // planes (1)
    info[12] = 1; info[13] = 0;
    // bits per pixel (24)
    info[14] = 24; info[15] = 0;
    // compression = 0 (no compression) → bytes 16..19 left at 0
    // imageSize (pixelDataSize) little-endian:
    info[20] = (uint8_t)(pixelDataSize      &0xFF);
    info[21] = (uint8_t)((pixelDataSize>> 8)&0xFF);
    info[22] = (uint8_t)((pixelDataSize>>16)&0xFF);
    info[23] = (uint8_t)((pixelDataSize>>24)&0xFF);
    // xresolution (pixels/meter) left 0
    // yresolution (pixels/meter) left 0
    // ncolors, importantcolors left 0

    fwrite(info, 1, 40, f);

    // --- PIXEL DATA + PADDING ---
    uint8_t pad[3] = {0,0,0};
    int padding    = rowSize - w*3;

    for (int y = h - 1; y >= 0; y--) {
        for (int x = 0; x < w; x++) {
            uint8_t bgr[3] = {
                img->data[y][x].blue,
                img->data[y][x].green,
                img->data[y][x].red
            };
            fwrite(bgr, 1, 3, f);
        }
        fwrite(pad, 1, padding, f);
    }

    fclose(f);
    printf("✅ Image saved to %s (%d×%d)\n", filename, w, h);
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
