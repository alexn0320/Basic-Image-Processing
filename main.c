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
    pixel** data = init_pixel_data(ih);

    read_bitmap("mountain.bmp", fh, ih, data);

    pixel** blur = init_pixel_data(ih);

    add_gaussian_blur(ih, data, blur, 1, 2);

    write_bitmap("out.bmp", fh, ih, blur);

    free_pixel_data(ih, data);

    return 0;
}
