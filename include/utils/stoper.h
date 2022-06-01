#ifndef __STOPER_H__
#define __STOPER_H__

#include <time.h>

typedef struct {
    struct timespec start;

    float delta; // in nanoseconds
} stoper_t;

void stoper_start(stoper_t *stoper);
void stoper_end(stoper_t *stoper);

#endif