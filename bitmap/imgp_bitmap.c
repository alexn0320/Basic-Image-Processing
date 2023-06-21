#include "imgp_bitmap.h"

void read_headers(const char* path, FILE_HEADER *fh, INFORMATION_HEADER *ih)
{
    FILE *f = fopen(path, "rb");

    //NULL check
    if(f == NULL)
    {
        printf("Error: file is NULL\n");
        return;
    }

    //the file headers is 14 bytes in size
    fread(fh, sizeof(FILE_HEADER), 1, f);

    //header field check
    if(fh->header_field != 0x4D42)
    {
        printf("Error: file is not a bitmap\n");
        return;
    }

    //the information headers is 40 bytes in size
    fread(ih, sizeof(INFORMATION_HEADER), 1, f);

    printf("<width> %d\n<height> %d\n", ih->bitmap_width, ih->bitmap_height);
    printf("<bits per pixel> %hu\n", ih->bits_per_pixel);

    fclose(f);
}