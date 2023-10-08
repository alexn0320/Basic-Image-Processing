#include "imgp_bitmap.h"

static int32_t min(int32_t a, int32_t b)
{
    if (a <= b)
        return a;

    return b;
}

uint8_t read_headers(const char *path, FILE_HEADER *fh, INFORMATION_HEADER *ih)
{
    FILE *f = fopen(path, "rb");

    // NULL check
    if (f == NULL)
    {
        printf("Error: file is NULL\n");
        return 0;
    }

    // the file headers is 14 bytes in size
    fread(fh, sizeof(FILE_HEADER), 1, f);

    // header field check
    if (fh->header_field != 0x4D42)
    {
        printf("Error: file is not a bitmap\n");
        return 0;
    }

    // the information header is 40 bytes in size
    fread(ih, sizeof(INFORMATION_HEADER), 1, f);

    printf("bitmap information:\n");
    printf("<width> %d\n<height> %d\n", ih->bitmap_width, ih->bitmap_height);
    printf("<bits per pixel> %hu\n", ih->bits_per_pixel);

    fclose(f);

    return 1;
}

pixel **init_pixel_data(INFORMATION_HEADER ih)
{
    // mallocs a double pointer
    pixel **data = malloc(ih.bitmap_height * sizeof(pixel *));

    for (uint32_t i = 0; i < ih.bitmap_height; i++)
        data[i] = malloc(ih.bitmap_width * sizeof(pixel));

    return data;
}

void free_pixel_data(INFORMATION_HEADER ih, pixel **data)
{
    for (uint32_t i = 0; i < ih.bitmap_height; i++)
        free(data[i]);

    free(data);
}

void read_bitmap(const char *path, FILE_HEADER fh, INFORMATION_HEADER ih, pixel **data)
{
    FILE *f = fopen(path, "rb");

    // sets the start of the reading to the image_data_offset (the offset of the pixel matrix)
    fseek(f, fh.image_data_offset, SEEK_SET);
    // the amount of bytes to read for every pixel
    size_t read_size = ih.bits_per_pixel >> 3;

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

    // aux values
    FILE_HEADER fh_aux;
    INFORMATION_HEADER ih_aux;

    size_t write_size = ih.bits_per_pixel >> 3;
    size_t data_size = write_size * ih.bitmap_height * ih.bitmap_width;

    // sets the headers of the bitmap
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

    // writes the headers
    fwrite(&fh_aux, sizeof(fh_aux), 1, f);
    fwrite(&ih_aux, sizeof(ih_aux), 1, f);

    // jumps to the pixel data start location
    fseek(f, fh_aux.image_data_offset, SEEK_SET);

    // writes the pixel data
    for (uint32_t i = 0; i < ih.bitmap_height; i++)
    {
        for (uint32_t j = 0; j < ih.bitmap_width; j++)
        {
            // swap RGBA to BGRA as the file format requires it
            swap_rb_values(&data[i][j]);
            fwrite(data[i] + j, write_size, 1, f);
            // reswap to RGBA
            swap_rb_values(data[i] + j);
        }
    }

    fclose(f);
}

void set_pixel(INFORMATION_HEADER ih, pixel *old, pixel new)
{
    memcpy(old, &new, sizeof(pixel));
}

void pixelate(INFORMATION_HEADER ih, pixel **data, uint8_t size)
{
    for (int i = 0; i < ih.bitmap_height; i += size)
    {
        for (int j = 0; j < ih.bitmap_width; j += size)
        {
            uint32_t average_r = 0, average_g = 0, average_b = 0;

            // get the average of an area size * size
            for (int ai = i; ai < min(ih.bitmap_height, i + size); ai++)
            {
                for (int aj = j; aj < min(ih.bitmap_width, j + size); aj++)
                {
                    average_r += data[ai][aj].r;
                    average_g += data[ai][aj].g;
                    average_b += data[ai][aj].b;
                }
            }

            // set all pixel values in that area to the average
            for (int ai = i; ai < min(ih.bitmap_height, i + size); ai++)
            {
                for (int aj = j; aj < min(ih.bitmap_width, j + size); aj++)
                {
                    data[ai][aj].r = average_r / (size * size);
                    data[ai][aj].g = average_g / (size * size);
                    data[ai][aj].b = average_b / (size * size);
                }
            }
        }
    }
}

void convert_grayscale(INFORMATION_HEADER ih, pixel **data)
{
    for (uint32_t i = 0; i < ih.bitmap_height; i++)
    {
        for (uint32_t j = 0; j < ih.bitmap_width; j++)
        {
            // constant values:
            // https://mmuratarat.github.io/2020-05-13/rgb_to_grayscale_formulas
            uint8_t aux = 0.299 * data[i][j].r + 0.587 * data[i][j].g + 0.114 * data[i][j].b;

            data[i][j].r = aux;
            data[i][j].g = aux;
            data[i][j].b = aux;
        }
    }
}

double **convolution(INFORMATION_HEADER ih, pixel **data, const uint8_t kernel_size, const double_t kernel[kernel_size][kernel_size], uint8_t channel)
{
    double_t **conv = malloc(ih.bitmap_height * sizeof(double_t *));

    for (uint32_t i = 0; i < ih.bitmap_height; i++)
        conv[i] = malloc(ih.bitmap_width * sizeof(double_t));

    for (uint32_t i = 0; i < ih.bitmap_height; i++)
        for (uint32_t j = 0; j < ih.bitmap_width; j++)
            conv[i][j] = 0;

    // convolution algorithm
    for (uint32_t i = 0; i < ih.bitmap_height - 2; i++)
    {
        for (uint32_t j = 0; j < ih.bitmap_width - 2; j++)
        {
            double_t val = 0;

            for (uint32_t ki = 0; ki < kernel_size; ki++)
            {
                for (uint32_t kj = 0; kj < kernel_size; kj++)
                {
                    // selects the proper channel based on parameter
                    switch (channel)
                    {
                    case 0:
                        val += data[i + ki][j + kj].r * kernel[ki][kj];
                        break;

                    case 1:
                        val += data[i + ki][j + kj].g * kernel[ki][kj];
                        break;

                    case 2:
                        val += data[i + ki][j + kj].b * kernel[ki][kj];
                        break;
                    }
                }
            }

            conv[i + 1][j + 1] = val;
        }
    }

    return conv;
}

pixel **convolution_RGB(INFORMATION_HEADER ih, pixel **data, const uint8_t kernel_size, const double_t kernel[kernel_size][kernel_size])
{
    // convolution for R channel
    pixel **conv = malloc(ih.bitmap_height * sizeof(pixel *));

    for (uint32_t i = 0; i < ih.bitmap_height; i++)
        conv[i] = malloc(ih.bitmap_width * sizeof(pixel));

    pixel empty = {.r = 0, .g = 0, .b = 0, .a = data[0][0].a, .cm = data[0][0].cm};

    for (uint32_t i = 0; i < ih.bitmap_height; i++)
        for (uint32_t j = 0; j < ih.bitmap_width; j++)
            conv[i][j] = empty;

    // convolution algorithm
    for (uint32_t i = 0; i < ih.bitmap_height - 2; i++)
    {
        for (uint32_t j = 0; j < ih.bitmap_width - 2; j++)
        {
            double_t val_r = 0, val_g = 0, val_b = 0;

            for (uint32_t ki = 0; ki < kernel_size; ki++)
            {
                for (uint32_t kj = 0; kj < kernel_size; kj++)
                {
                    val_r += data[i + ki][j + kj].r * kernel[ki][kj];
                    val_g += data[i + ki][j + kj].g * kernel[ki][kj];
                    val_b += data[i + ki][j + kj].b * kernel[ki][kj];
                }
            }

            //clamp values to 255
            val_r = min(val_r, 255);
            val_g = min(val_g, 255);
            val_b = min(val_b, 255);

            conv[i + 1][j + 1].r = val_r;
            conv[i + 1][j + 1].g = val_g;
            conv[i + 1][j + 1].b = val_b;
        }
    }

    return conv;
}

void sobel(INFORMATION_HEADER ih, pixel **data)
{
    double_t **conv_x = convolution(ih, data, 3, SOBEL_KX, 0);
    double_t **conv_y = convolution(ih, data, 3, SOBEL_KY, 0);

    for (uint32_t i = 1; i < ih.bitmap_height - 1; i++)
    {
        for (uint32_t j = 1; j < ih.bitmap_width - 1; j++)
        {
            double mag = sqrt(conv_x[i][j] * conv_x[i][j] + conv_y[i][j] * conv_y[i][j]);

            if (mag > 255)
                mag = 255;

            data[i][j].r = mag;
            data[i][j].g = mag;
            data[i][j].b = mag;
        }
    }

    for (uint32_t i = 0; i < ih.bitmap_height; i++)
        free(conv_x[i]);

    free(conv_x);

    for (uint32_t i = 0; i < ih.bitmap_height; i++)
        free(conv_y[i]);

    free(conv_y);
}