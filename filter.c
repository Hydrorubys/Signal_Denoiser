#include <math.h>
#include "filter.h"

void harmonic_filter(
    Complex *spectrum,
    int n,
    float fe,
    float f0,
    float tolerance,
    int nb_harmonics
){
    for (int k = 0; k < n; k++) {

        // Fréquence associée au coefficient
        float frequency;

        if (k <= n / 2)
            frequency = (float)k * fe / n;
        else
            frequency = (float)(k - n) * fe / n;

        frequency = fabsf(frequency);

        // Recherche d'une harmonique proche
        int keep = 0;

        for (int h = 1; h <= nb_harmonics; h++) {

            float harmonic = h * f0;

            if (fabsf(frequency - harmonic) <= tolerance) {
                keep = 1;
                break;
            }
        }

        // Suppression des fréquences non conservées
        if (!keep) {
            spectrum[k].re = 0.0f;
            spectrum[k].im = 0.0f;
        }
    }
}
