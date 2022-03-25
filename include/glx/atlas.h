#ifndef __ATLAS_H__
#define __ATLAS_H__

typedef struct {
    unsigned ID;

    unsigned type;
    unsigned tile_w, tile_h;
} atlas_t;

atlas_t* atlas_generate(const char *path, unsigned tile_w, unsigned tile_h);
void atlas_bind(atlas_t *p);
void atlas_destroy(atlas_t *p);

#endif