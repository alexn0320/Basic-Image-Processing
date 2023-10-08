#include <stdio.h>
#include <string.h>
#include "util/imgp_util.h"
#include "bitmap/imgp_bitmap.h"

/*
    Basic image editor in C
    A rewrite to fix issues, add more functionality and merge color model functions
*/

int main(int argc, char **argv)
{
    FILE_HEADER fh;
    INFORMATION_HEADER ih;

    char *file_name = malloc(128 * sizeof(char));
    strcpy(file_name, "input");

    if (argc == 2)
        strcpy(file_name, argv[1]);

    char *full_path = malloc(256 * sizeof(char));

    strcpy(full_path, "images/");
    strcat(full_path, file_name);
    strcat(full_path, ".bmp");

    printf("File: %s\n", full_path);

    uint8_t flag = read_headers(full_path, &fh, &ih);

    if (flag == 0)
    {
        free(file_name);
        free(full_path);
        
        return 0;
    }

    pixel **data = init_pixel_data(ih);
    read_bitmap(full_path, fh, ih, data);

    pixel** conv_data = convolution_RGB(ih, data, 3, GAUSSIAN_BLUR_3X3);

    write_bitmap("images/out1.bmp", fh, ih, conv_data);

    free_pixel_data(ih, data);
    free_pixel_data(ih, conv_data);
    free(file_name);
    free(full_path);

    return 0;
}
