#ifndef IMGP_BITMAP
#define IMGP_BITMAP

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../util/imgp_util.h"

//adds PI constant from math.h
#define __USE_MISC
#include <math.h>

/*
    Contains all the major functions used to manipulate bitmap images
*/

//reads the headers of the bitmap file
extern uint8_t read_headers(const char* path, FILE_HEADER *fh, INFORMATION_HEADER *ih);
//inits the pixel matrix
extern pixel** init_pixel_data(INFORMATION_HEADER ih);
//frees the pixel matrix
extern void free_pixel_data(INFORMATION_HEADER ih, pixel** data);
//reads the data of a bitmap 
extern void read_bitmap(const char *path, FILE_HEADER fh, INFORMATION_HEADER ih, pixel** data);
//writes the data to a bitmap file
extern void write_bitmap(const char *path, FILE_HEADER fh, INFORMATION_HEADER ih, pixel** data);
//set the pixel at pos p to the new pixel
extern void set_pixel(INFORMATION_HEADER ih, pixel* old, pixel new);
//pixelates the image. size represents the actual size of the pixels
extern void pixelate(INFORMATION_HEADER ih, pixel** data, uint8_t size);
//convolution function. It operates on one channel at a time, specified by the channel param:
//0 => R, 1 => G, 2 => B.
extern double** convolution(INFORMATION_HEADER ih, pixel** data, const uint8_t kernel_size, const double_t kernel[kernel_size][kernel_size], uint8_t channel);
//convolves all RGB channels instead of one. Can be used for filters such as blur.
extern pixel** convolution_RGB(INFORMATION_HEADER ih, pixel** data, const uint8_t kernel_size, const double_t kernel[kernel_size][kernel_size]);
//converts the pixel data to grayscale
extern void convert_grayscale(INFORMATION_HEADER ih, pixel** data);
//sobel filter implementation
extern void sobel(INFORMATION_HEADER ih, pixel** data);
#endif