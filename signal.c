#include <math.h>
#include <stdlib.h>
#include "signal.h"

#define PI 3.14159265358979323846

// Bruit blanc gaussien par méthode de Box-Muller
static float gaussian_noise(void)
{
    float u1 = ((float)rand() + 1.0f) / (RAND_MAX + 2.0f);
    float u2 = ((float)rand() + 1.0f) / (RAND_MAX + 2.0f);

    return sqrtf(-2.0f * logf(u1)) * cosf(2.0f * PI * u2);
}

void generate_signal(
    float *signal,
    float *clean,
    float *noise,
    int n,
    float fe
)
{
    const float f0 = 110.0f;
    const float sigma = 0.15f;

    // Amplitudes des 6 premières harmoniques
    const float amplitudes[] = {
        1.0f, 0.5f, 0.3f, 0.2f, 0.12f, 0.08f
    };

    for (int i = 0; i < n; i++) {

        float t = (float)i / fe;
        float s = 0.0f;

        for (int k = 1; k <= 6; k++) {
            s += amplitudes[k - 1]
                * sinf(2.0f * PI * k * f0 * t);
        }

        clean[i] = s;

        noise[i] = sigma * gaussian_noise();

        signal[i] = clean[i] + noise[i];
    }
}
