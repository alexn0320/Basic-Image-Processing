#ifndef IMGP_BITMAP
#define IMGP_BITMAP

#include <stdio.h>
#include "../util/imgp_util.h"

/*
    Contains all the major functions used to manipule the images
*/

//reads the headers of the bitmap file
extern void read_headers(const char* path, FILE_HEADER *fh, INFORMATION_HEADER *ih);

#endif