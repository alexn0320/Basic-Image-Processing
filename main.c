#include <stdio.h>
#include <string.h>
#include "util/imgp_util.h"
#include "bitmap/imgp_bitmap.h"

int main()
{
    FILE_HEADER fh;
    INFORMATION_HEADER ih;

    read_headers("mountain.bmp", &fh, &ih);

    return 0;
}
