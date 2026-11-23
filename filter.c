
#include <math.h>
#include "filter.h"

#define MAX_TOLERANCE 50.0

void harmonic_filter(
    Complex *spectrum,
    int n,
    float fe,
    float f0,
    float tolerance,
    int nb_harmonics,
    float alpha
) {
    for (int k = 0; k < n; k++) {

        float frequency;

        if (k <= n / 2)
            frequency = (float)k * fe / n;
        else
            frequency = (float)(k - n) * fe / n;

        frequency = fabsf(frequency);

        int keep = 0;

        for (int h = 1; h <= nb_harmonics; h++) {

            float harmonic = h * f0;
            float tolerance_h = tolerance + alpha * (h - 1);

            if (tolerance_h > MAX_TOLERANCE)
                tolerance_h = MAX_TOLERANCE;

            if (fabsf(frequency - harmonic) <= tolerance_h) {
                keep = 1;
                break;
            }
        }

        if (!keep) {
            spectrum[k].re = 0.0f;
            spectrum[k].im = 0.0f;
        }
    }
}
