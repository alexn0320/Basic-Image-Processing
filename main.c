#include <stdio.h>
#include <string.h>
#include "util/imgp_util.h"
#include "bitmap/imgp_bitmap.h"

/*
    Basic image editor in C
    A rewrite to fix issues, add more functionality and merge color model functions 
*/

int main()
{
    FILE_HEADER fh;
    INFORMATION_HEADER ih;

    read_headers("mountain.bmp", &fh, &ih);

    return 0;
}
