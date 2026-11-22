#ifndef FFT_H
#define FFT_H

typedef struct {
    float re;
    float im;
} Complex;

void fft(Complex *x, int n);
void ifft(Complex *x, int n);

#endif
