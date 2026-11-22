#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "fft.h"
#include "signal.h"
#include "filter.h"

#define N 2048
#define FE 44100.0f
#define PI 3.14159265358979323846

// source .venv/bin/activate
//gcc -O2 main.c fft.c signal.c filter.c -o denoiser -lm

int main(void)
{
    float clean[N];
    float noise[N];
    float signal[N];
    float reconstructed[N];

    Complex spectrum[N];

    srand(42);

    // 1. Génération du signal
    generate_signal(signal, clean, noise, N, FE);

    // 2. Conversion en nombres complexes
    for (int i = 0; i < N; i++) {
        spectrum[i].re = signal[i];
        spectrum[i].im = 0.0f;
    }

    // 3. FFT
    fft(spectrum, N);

    // 4. Sauvegarde du spectre
    FILE *fs = fopen("spectrum.csv", "w");

    if (fs == NULL) {
        perror("spectrum.csv");
        return 1;
    }

    fprintf(fs, "frequency,magnitude\n");

    for (int k = 0; k <= N / 2; k++) {

        float frequency = (float)k * FE / N;

        float magnitude = sqrtf(
            spectrum[k].re * spectrum[k].re
            + spectrum[k].im * spectrum[k].im
        );

        fprintf(fs, "%f,%f\n", frequency, magnitude);
    }

    fclose(fs);

    // 5. Filtrage harmonique
    harmonic_filter(
        spectrum,
        N,
        FE,
        110.0f,  // fréquence fondamentale
        1064.0f,   // tolérance en Hz
        6        // nombre d'harmoniques
    );

    // 6. FFT inverse
    ifft(spectrum, N);

    for (int i = 0; i < N; i++)
        reconstructed[i] = spectrum[i].re;

    // 7. Calcul de l'erreur de reconstruction
    double error = 0.0;
    double energy = 0.0;
    double signal_energy = 0.0;
    double noise_energy = 0.0;
    double filtered_error_energy = 0.0;

    for (int i = 0; i < N; i++) {

        double e = signal[i] - reconstructed[i];

        error += e * e;
        energy += signal[i] * signal[i];

        signal_energy += clean[i] * clean[i];
        noise_energy += noise[i] * noise[i];

        double e_filtered = clean[i] - reconstructed[i];
        filtered_error_energy += e_filtered * e_filtered;
    }

    double snr = 10.0 * log10(
        signal_energy / noise_energy
    );

    double snr_filtered = 10.0 * log10(
        signal_energy / filtered_error_energy
    );

    printf("SNR initial : %.2f dB\n", snr);
    printf("SNR apres filtrage : %.2f dB\n", snr_filtered);
    printf("Gain de SNR : %.2f dB\n", snr_filtered - snr);

    printf("Erreur relative de reconstruction : %.12e\n",
        sqrt(error / energy));

    // 8. Export des signaux temporels
    FILE *ft = fopen("signal.csv", "w");

    if (ft == NULL) {
        perror("signal.csv");
        return 1;
    }

    fprintf(ft,
        "time,clean,noise,noisy,reconstructed\n"
    );

    for (int i = 0; i < N; i++) {

        float t = (float)i / FE;

        fprintf(ft, "%f,%f,%f,%f,%f\n",
            t,
            clean[i],
            noise[i],
            signal[i],
            reconstructed[i]
        );
    }

    fclose(ft);

    printf("Fichiers CSV generes.\n");

    return 0;
}
