#ifndef IMGP_BITMAP
#define IMGP_BITMAP

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../util/imgp_util.h"

/*
    Contains all the major functions used to manipule the images
*/

//reads the headers of the bitmap file
extern void read_headers(const char* path, FILE_HEADER *fh, INFORMATION_HEADER *ih);
//inits the pixel matrix
extern pixel** init_pixel_data(INFORMATION_HEADER ih);
//reads the data of a bitmap 
extern void read_bitmap(const char *path, FILE_HEADER fh, INFORMATION_HEADER ih, pixel** data);
//writes the data to a bitmap file
extern void write_bitmap(const char *path, FILE_HEADER fh, INFORMATION_HEADER ih, pixel** data);
#endif