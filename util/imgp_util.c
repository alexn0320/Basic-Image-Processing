#include "imgp_util.h"

void swap_rb_values(pixel *p)
{
    BYTE aux;

    aux = p->r;
    p->r = p->b;
    p->b = aux;
}