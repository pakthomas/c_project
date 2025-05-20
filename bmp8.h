//
// Created by Thomas on 3/21/2025.
//

#ifndef BMP8_H
#define BMP8_H
//definition of a structured type t_bmp8
// Offsets for the BMP header
#define BITMAP_MAGIC 0x00 // offset 0
#define BITMAP_SIZE 0x02 // offset 2
#define BITMAP_OFFSET 0x0A // offset 10
18
24-bit Image and Color Image Processing
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
    t_bmp_header header;
    t_bmp_info header_info;
    int width;
    int height;
    int colorDepth;
    t_pixel **data;
} t_bmp24;
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset; } t_bmp_header;
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

//definition of allocation and deallocation functions
t_pixel ** bmp24_allocateDataPixels (int width, int height);
void bmp24_freeDataPixels (t_pixel ** pixels, int height);
t_bmp24 * bmp24_allocate (int width, int height, int colorDepth);
void bmp24_free (t_bmp24 * img);

//Convulition filers
t_pixel bmp24_convolution (t_bmp24 * img, int x, int y, float ** kernel, int kernelSize);

//read and write pixel data
void bmp24_readPixelValue (t_bmp24 * image, int x, int y, FILE * file);
void bmp24_readPixelData (t_bmp24 * image, FILE * file);
void bmp24_writePixelValue (t_bmp24 * image, int x, int y, FILE * file);
void bmp24_writePixelData (t_bmp24 * image, FILE * file);


//Loading and saving 24-bit Images
t_bmp24 * bmp24_loadImage (const char * filename);
void bmp24_saveImage (t_bmp24 * img, const char * filename);

//Get access to header in 24 bytes image
void file_rawRead (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file)

#endif
