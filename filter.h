#ifndef FILTER_H
#define FILTER_H

#include "fft.h"

void harmonic_filter(
    Complex *spectrum,
    int n,
    float fe,
    float f0,
    float tolerance,
    int nb_harmonics,
    float alpha
);

#endif
