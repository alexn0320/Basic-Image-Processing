#ifndef IMGP_UTIL
#define IMGP_UTIL

#include <stdint.h>

/*
    util header and source file that defines basic elements of the .bmp file format (image headers, pixel representation),
    used color models and actions related to them
*/

enum COLOR_MODEL {RGB, RGBA, grayscale};

//basic typedefs
typedef unsigned char BYTE;
typedef int32_t LONG;
typedef uint32_t DWORD;
typedef short WORD;

//pixel color model (supports RGB, RGBA, grayscale)
typedef struct
{
    BYTE r,g,b,a;
    enum COLOR_MODEL cm;
}__attribute__((packed)) pixel;

//position in image (x, y)
typedef struct 
{
    LONG x, y;
} pos;

//Bitmap file format: https://en.wikipedia.org/wiki/BMP_file_format
//first image header (file header)
typedef struct
{
    //header field (should be BMP)
    WORD header_field;
    //whole bmp file size
    DWORD bmp_file_size;
    //reserved values (not used)
    WORD reserved_1;
    WORD reserved_2;
    //offset to the pixel matrix 
    DWORD image_data_offset;
}__attribute__((packed)) FILE_HEADER;

//second image header (information header)
typedef struct 
{
    //40 bytes
    DWORD header_size;
    //pixel matrix width and height
    LONG bitmap_width;
    LONG bitmap_height;
    //number of color planes (should be 1)
    WORD color_planes;
    //number of bits per pixel 
    WORD bits_per_pixel;
    //compression method used
    DWORD compression;
    DWORD raw_bitmap_size;
    //horizontal and vertical resolutions
    LONG horizontal_res;
    LONG vertical_res;
    //color pallete used
    DWORD color_pallete;
    //important colors (can be set to 1)
    DWORD important_colors;
}__attribute__((packed)) INFORMATION_HEADER;

//bitmaps store pixels as BGRA and so R and B values must be swapped
extern void swap_rb_values(pixel *p);

#endif