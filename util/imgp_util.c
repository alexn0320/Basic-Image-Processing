#include "imgp_util.h"

const double BOX_BLUR[3][3] = {1 / 9.0, 1 / 9.0, 1 / 9.0,
                               1 / 9.0, 1 / 9.0, 1 / 9.0,
                               1 / 9.0, 1 / 9.0, 1 / 9.0};

const double GAUSSIAN_BLUR_3X3[3][3] = {1 / 16.0, 2 / 16.0, 1 / 16.0,
                                        2 / 16.0, 4 / 16.0, 2 / 16.0,
                                        1 / 16.0, 2 / 16.0, 1 / 16.0};

const double SOBEL_KX[3][3] = {1, 0, -1,
                              2, 0, -2,
                              1, 0, -1};

const double SOBEL_KY[3][3] = {1, 2, 1,
                              0, 0, 0,
                              -1, -2, -1};

void swap_rb_values(pixel *p)
{
    BYTE aux;

    aux = p->r;
    p->r = p->b;
    p->b = aux;
}