#include "utils/font_loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

int str_to_int(const char *p) {
    int n = 0;

    while(*p >= '0' && *p <= '9') {
        n = n * 10 + (*p - '0');
        ++p;
    }

    return n;
}

fontbmp_t* fontbmp_make(const char *path) {
    fontbmp_t *temp = (fontbmp_t*)malloc(sizeof(fontbmp_t));
    if(!temp) {
        fprintf(stderr, "Couldn't allocate memory for fontbmp_t\n");
        return NULL;
    }

    FILE *info = fopen(path, "rb");
    if(!info) {
        fprintf(stderr, "Couldn't open %s\n", path);
        return NULL;
    }

    const unsigned SIZE = 256;
    char buffer[SIZE];

    char file_name[SIZE / 2];
    int file_name_size = 0;

    while(fgets(buffer, SIZE, info)) {
        char *found = NULL;
        if((found = strstr(buffer, "file")) != NULL) {
            int first = 6;
            int last = first;

            for(; found[last] != '"'; ++last);
            file_name_size = last - first;

            strncpy(file_name, found + first, file_name_size);
            file_name[file_name_size] = 0;

            printf("Filename: %s\n", file_name);
        }

        if((found = strstr(buffer, "chars count")) != NULL) {
            temp->count = str_to_int(found + 12);
            printf("Chars count: %d\n", temp->count);
            break;
        }
    }

    temp->chars = (charbmp_t*)malloc(temp->count * sizeof(charbmp_t));
    if(!temp->chars) {
        fprintf(stderr, "Couldn't allocate memory for font chars\n");
        return NULL;
    }

    for(int i = 0; i < temp->count; ++i) {
        charbmp_t *c = &temp->chars[i];
        char *found = NULL;

        fgets(buffer, SIZE, info);
        
        found = strstr(buffer, "id");
        c->ID = str_to_int(found + 3);

        found = strstr(buffer, "x");
        c->x = str_to_int(found + 2);

        found = strstr(buffer, "y");
        c->y = str_to_int(found + 2);

        found = strstr(buffer, "width");
        c->w = str_to_int(found + 6);

        found = strstr(buffer, "height");
        c->h = str_to_int(found + 7);

        printf("Character %d x=%d y=%d w=%d h=%d\n", c->ID, c->x, c->y, c->w, c->h);
    }

    fclose(info);

    int last_slash = 0;
    for(int i = 0; path[i] != 0; ++i) {
        if(path[i] == '/') last_slash = i;
    }

    char texture_path[SIZE];
    strncpy(texture_path, path, last_slash + 1);
    // + 1 to file_name_size, to copy 0 as well (TODO: junky solution, change it later)
    strncpy(texture_path + last_slash + 1, file_name, file_name_size + 1);
    printf("Texture path: %s\n", texture_path);

    temp->texture = texture_make(texture_path);

    return temp;
}

void fontbmp_close(fontbmp_t *p) {
    if(!p) return;

    if(p->texture) texture_destroy(p->texture);
    if(p->chars) free(p->chars);

    free(p);
}