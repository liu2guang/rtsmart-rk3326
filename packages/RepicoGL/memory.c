/*
 * Memory allocator for PicoGL
 */
#include <stdlib.h>

/* modify these functions so that they suit your needs */

void gl_free(void *p)
{
    free(p);
}

void *gl_malloc(int size)
{
    return malloc(size);
}

void *gl_zalloc(int size)
{
    return calloc(1, size);
}
