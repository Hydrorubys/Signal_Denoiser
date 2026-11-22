#include <math.h>
#include "fft.h"

#define PI 3.14159265358979323846

static void swap(Complex *a, Complex *b)
{
    Complex tmp = *a;
    *a = *b;
    *b = tmp;
}

static void transform(Complex *x, int n, int inverse)
{
    // Permutation bit-reversal
    int j = 0;

    for (int i = 1; i < n; i++) {
        int bit = n >> 1;

        while (j & bit) {
            j ^= bit;
            bit >>= 1;
        }

        j ^= bit;

        if (i < j)
            swap(&x[i], &x[j]);
    }

    // Cooley-Tukey
    for (int len = 2; len <= n; len <<= 1) {

        float angle = 2.0f * PI / len;

        if (inverse)
            angle = -angle;

        float wlen_re = cosf(angle);
        float wlen_im = sinf(angle);

        for (int i = 0; i < n; i += len) {

            float w_re = 1.0f;
            float w_im = 0.0f;

            for (int k = 0; k < len / 2; k++) {

                Complex u = x[i + k];
                Complex v = x[i + k + len / 2];

                float t_re = v.re * w_re - v.im * w_im;
                float t_im = v.re * w_im + v.im * w_re;

                x[i + k].re = u.re + t_re;
                x[i + k].im = u.im + t_im;

                x[i + k + len / 2].re = u.re - t_re;
                x[i + k + len / 2].im = u.im - t_im;

                float next_re = w_re * wlen_re - w_im * wlen_im;
                float next_im = w_re * wlen_im + w_im * wlen_re;

                w_re = next_re;
                w_im = next_im;
            }
        }
    }

    // Normalisation de la transformée inverse
    if (inverse) {
        for (int i = 0; i < n; i++) {
            x[i].re /= n;
            x[i].im /= n;
        }
    }
}

void fft(Complex *x, int n)
{
    transform(x, n, 0);
}

void ifft(Complex *x, int n)
{
    transform(x, n, 1);
}
