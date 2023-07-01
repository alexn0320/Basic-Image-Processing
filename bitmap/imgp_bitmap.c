#include "imgp_bitmap.h"

void read_headers(const char *path, FILE_HEADER *fh, INFORMATION_HEADER *ih)
{
    FILE *f = fopen(path, "rb");

    // NULL check
    if (f == NULL)
    {
        printf("Error: file is NULL\n");
        return;
    }

    // the file headers is 14 bytes in size
    fread(fh, sizeof(FILE_HEADER), 1, f);

    // header field check
    if (fh->header_field != 0x4D42)
    {
        printf("Error: file is not a bitmap\n");
        return;
    }

    // the information header is 40 bytes in size
    fread(ih, sizeof(INFORMATION_HEADER), 1, f);

    printf("bitmap information:\n");
    printf("<width> %d\n<height> %d\n", ih->bitmap_width, ih->bitmap_height);
    printf("<bits per pixel> %hu\n", ih->bits_per_pixel);

    fclose(f);
}

pixel **init_pixel_data(INFORMATION_HEADER ih)
{
    // mallocs a double pointer
    pixel **data = malloc(ih.bitmap_height * sizeof(pixel *));

    for (uint32_t i = 0; i < ih.bitmap_height; i++)
        data[i] = malloc(ih.bitmap_width * sizeof(pixel));

    return data;
}

void free_pixel_data(INFORMATION_HEADER ih, pixel** data)
{
    for(uint32_t i = 0; i < ih.bitmap_height; i++)
        free(data[i]);

    free(data);
}

void read_bitmap(const char *path, FILE_HEADER fh, INFORMATION_HEADER ih, pixel **data)
{
    FILE *f = fopen(path, "rb");

    // sets the start of the reading to the image_data_offset (the offset of the pixel matrix)
    fseek(f, fh.image_data_offset, SEEK_SET);
    // the amount of bytes to read for every pixel
    size_t read_size = ih.bits_per_pixel / 8;

    switch (ih.bits_per_pixel)
    {
    case 24:

        for (uint32_t i = 0; i < ih.bitmap_height; i++)
        {
            for (uint32_t j = 0; j < ih.bitmap_width; j++)
            {
                fread(data[i] + j, read_size, 1, f);
                swap_rb_values(data[i] + j);

                data[i][j].cm = RGB;
            }
        }

        break;

    case 32:
        for (uint32_t i = 0; i < ih.bitmap_height; i++)
        {
            for (uint32_t j = 0; j < ih.bitmap_width; j++)
            {
                fread(data[i] + j, read_size, 1, f);
                swap_rb_values(data[i] + j);

                data[i][j].cm = RGBA;
            }
        }

    default:
        break;
    }

    fclose(f);
}

void write_bitmap(const char *path, FILE_HEADER fh, INFORMATION_HEADER ih, pixel **data)
{
    FILE *f = fopen(path, "wb");

    //aux values
    FILE_HEADER fh_aux;
    INFORMATION_HEADER ih_aux;

    size_t write_size = ih.bits_per_pixel / 8;
    size_t data_size = write_size * ih.bitmap_height * ih.bitmap_width;

    //sets the headers of the bitmap
    fh_aux.header_field = 0x4D42;
    fh_aux.bmp_file_size = sizeof(ih) + sizeof(fh) + data_size;
    fh_aux.reserved_1 = 0;
    fh_aux.reserved_2 = 0;
    fh_aux.image_data_offset = fh.bmp_file_size - data_size;

    ih_aux.header_size = 40;
    ih_aux.bitmap_width = ih.bitmap_width;
    ih_aux.bitmap_height = ih.bitmap_height;
    ih_aux.color_planes = 1;
    ih_aux.bits_per_pixel = ih.bits_per_pixel;
    ih_aux.compression = 0;
    ih_aux.raw_bitmap_size = data_size;
    ih_aux.horizontal_res = 0;
    ih_aux.vertical_res = 0;
    ih_aux.color_pallete = 0;
    ih_aux.important_colors = 0;

    //writes the headers
    fwrite(&fh_aux, sizeof(fh_aux), 1, f);
    fwrite(&ih_aux, sizeof(ih_aux), 1, f);

    //jumps to the pixel data start location
    fseek(f, fh_aux.image_data_offset, SEEK_SET);

    //writes the pixel data
    for (uint32_t i = 0; i < ih.bitmap_height; i++)
    {
        for (uint32_t j = 0; j < ih.bitmap_width; j++)
        {
            //swap RGBA to BGRA as the file format requires it
            swap_rb_values(&data[i][j]);
            fwrite(data[i] + j, write_size, 1, f);
            //reswap to RGBA
            swap_rb_values(data[i] + j);
        }
    }

    fclose(f);
}

void set_pixel(INFORMATION_HEADER ih, pixel* old, pos p, pixel new)
{
    //basic error checking
    if(ih.bitmap_width < p.x || p.x < 0)
    {
        printf("Error: invalid position\n");
        return;
    }

    if(ih.bitmap_height < p.y || p.y < 0)
    {
        printf("Error: invalid position\n");
        return;
    }

    old->r = new.r;
    old->g = new.g;
    old->b = new.b;
    old->a = new.a;
    old->cm = new.cm;
}