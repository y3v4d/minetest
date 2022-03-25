#ifndef __IMG_LOADER_H__
#define __IMG_LOADER_H__

typedef unsigned char byte_t;

typedef struct {
    unsigned width, height;

    byte_t *data;
} bmp_t;

bmp_t* load_bmp(const char *path, int use_alpha, int reverse);
void free_bmp(bmp_t *p);

#endif
