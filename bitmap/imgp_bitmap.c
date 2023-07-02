#include "imgp_bitmap.h"

// Sobel kernels from image convolution
const double_t K_X[3][3] = {-1, 0, 1,
                            -2, 0, 2 - 1, 0, 1};

const double_t K_Y[3][3] = {1, 2, 1,
                            0, 0, 0 - 1, -2, -1};

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

    // aux values
    FILE_HEADER fh_aux;
    INFORMATION_HEADER ih_aux;

    size_t write_size = ih.bits_per_pixel / 8;
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

void set_pixel(INFORMATION_HEADER ih, pixel *old, pos p, pixel new)
{
    // basic error checking
    if (ih.bitmap_width < p.x || p.x < 0)
    {
        printf("Error: invalid position\n");
        return;
    }

    if (ih.bitmap_height < p.y || p.y < 0)
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

void add_gaussian_blur(INFORMATION_HEADER ih, pixel **data, pixel **new_data, int8_t radius, int8_t sigma)
{
    //kernel data 
    double_t kernel_div = 16.0;
    int8_t kernel_size = radius << 1 + 1;

    double_t** kernel = malloc(kernel_size * sizeof(double_t*));

    for(uint8_t i = 0; i < kernel_size; i++)
        kernel[0] = malloc(kernel_size * sizeof(double_t));

    double_t sum = 0.0;

    //generates kernel using formula
    for (int32_t i = -radius; i < radius; i++)
    {
        for (int32_t j = -radius; j < radius; j++)
        {
            double_t num = (double_t)(-(i * i + j * j));
            double den = 2 * sigma * sigma;

            double expr = exp(num / den);

            // get kernel value
            double kernel_value = expr / (2 * M_PI * sigma * sigma);

            kernel[i + radius][j + radius] = kernel_value;
            sum += kernel_value;
        }
    }

    // normates values to 1
    for (int32_t i = -radius; i < radius; i++)
        for (int32_t j = -radius; j < radius; j++)
            kernel[i + radius][j + radius] = kernel[i + radius][j + radius] / sum;

    convolution(ih, kernel_size, kernel, data, new_data);
}

void convolution(INFORMATION_HEADER ih, int8_t kernel_size, double_t** kernel, pixel **data, pixel **conv_data)
{
    int8_t k = (kernel_size - 1) >> 1;

    for (int32_t i = 0; i < ih.bitmap_height; i++)
    {
        for (int32_t j = 0; j < ih.bitmap_width; j++)
        {
            double_t r = 0.0, g = 0.0, b = 0.0;

            // traverse the kernel to obtain the weighted value of the pixel
            for (int32_t kx = -k; kx < k; kx++)
            {
                for (int32_t ky = -k; ky < k; ky++)
                {
                    double_t kernel_value = kernel[kx + k][ky + k];

                    if ((i - ky > 0 && i - ky < ih.bitmap_height) && (j - kx > 0 && j - kx < ih.bitmap_width))
                    {
                        r += data[i - ky][j - kx].r * kernel_value;
                        g += data[i - ky][j - kx].g * kernel_value;
                        b += data[i - ky][j - kx].b * kernel_value;
                    }
                }
            }

            conv_data[i][j].r = (BYTE) (r);
            conv_data[i][j].g = (BYTE) (g);
            conv_data[i][j].b = (BYTE) (b);
        }
    }
}