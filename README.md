# Image Processing in C

This project is part of the TI202I - Algorithmic and Data Structures 1 module. It focuses on implementing image processing techniques in C for both grayscale and color images using the BMP format. This project further familiarizes students to the C language.

## Project Overview

This program allows you to:
- Read and write 8-bit and 24-bit BMP images.
- Apply various image processing operations (e.g., inversion, brightness adjustment, thresholding).
- Apply convolution filters (e.g blur, sharpen, emboss via kernel multiplication).
- Perform histogram equalization to improve contrast.
- all of these features come with a very ergonomic menu with error messages if necessary.

## Menu
When first executing the code, you will be greeted by the following message:
- **Please choose an option:**
1. Open an image
2. Save an image
3. Apply a filter
4. Display image information
5. Quit

In order to select any of these options, you must write any number ranging from 1 to 5 in the terminal then press the button 'Enter'. The code is robust enough to be able to receive any input as long as it contains a number (i.e the input "**space** 1", will be treated by the code the same way as the input "1")
You must open an image before applying a filter.


When pressing 1, you will get the following question :\
**Is the image:**

0. Grayscale (8-bit)
1. Color (24-bit)

After loading the image, you will gain access to many features such as:
Extracting the data of an image by pressing 3:
example (Lena_gray.bmp) : \
Image Info:
Width: 512
Height: 512
Color Depth: 8
Data Size: 262144

being able to apply a filter on the image by selecting the "Apply filter" option.
You will then receive this message : \
**Please choose a filter:**
1. Negative
2. Brightness
3. Black and white (threshold / grayscale)
4. Box Blur
5. Gaussian blur
6. Sharpness
7. Outline
8. Emboss
9. Return to the previous menu 


Each filter modifies the image in some way or another, and we highly recommend the user to try every possibility!

