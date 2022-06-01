#include "stoper.h"

void stoper_start(stoper_t *stoper) {
    clock_gettime(CLOCK_REALTIME, &stoper->start);
    stoper->delta = 0;
}

void stoper_end(stoper_t *stoper) {
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);

    const time_t start_n = stoper->start.tv_sec * 1000000000 + stoper->start.tv_nsec;
    const time_t end_n = end.tv_sec * 1000000000 + end.tv_nsec;

    stoper->delta = (float)(start_n - end_n) / 1000000;
}