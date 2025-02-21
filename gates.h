#ifndef GATES_H
#define GATES_H

#include <complex.h>

#define N 2

const double complex H[N*N] = {
    1.0 / csqrt(2),  1.0 / csqrt(2),
    1.0 / csqrt(2), -1.0 / csqrt(2)
};

const double complex In[N*N] = {
    1, 0,
    0, 1
};

const double complex Not[N*N] = {
    0, 1,
    1, 0
};

#endif // GATES_H
