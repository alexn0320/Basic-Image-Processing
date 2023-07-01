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
    Contains all the major functions used to manipule the images
*/

//reads the headers of the bitmap file
extern void read_headers(const char* path, FILE_HEADER *fh, INFORMATION_HEADER *ih);
//inits the pixel matrix
extern pixel** init_pixel_data(INFORMATION_HEADER ih);
//frees the pixel matrix
extern void free_pixel_data(INFORMATION_HEADER ih, pixel** data);
//reads the data of a bitmap 
extern void read_bitmap(const char *path, FILE_HEADER fh, INFORMATION_HEADER ih, pixel** data);
//writes the data to a bitmap file
extern void write_bitmap(const char *path, FILE_HEADER fh, INFORMATION_HEADER ih, pixel** data);
//set the pixel at pos p to the new pixel
extern void set_pixel(INFORMATION_HEADER ih, pixel* old, pos p, pixel new);
#endif