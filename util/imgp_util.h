#ifndef IMGP_UTIL
#define IMGP_UTIL

/*
    util header and source file that defines basic elements of the .bmp file format (image headers, pixel representation)
    and actions related to them
*/

//basic typedefs
typedef unsigned char BYTE;
typedef int LONG;
typedef unsigned int DWORD;
typedef short WORD;

//pixel definition (supports RGB, RGBA, grayscale)
typedef struct
{
    BYTE r,g,b,a;
}__attribute__((packed)) pixel;

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

#endif