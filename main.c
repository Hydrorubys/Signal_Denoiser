
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <sndfile.h>

#include "yin.h"
#include "fft.h"
#include "filter.h"

#define FE 44100
#define W 1024
#define H 256

#define TAU_MIN 44
#define TAU_MAX 735
#define YIN_THRESHOLD 0.15

#define NB_HARMONICS 24

// Une largeur d'environ un intervalle fréquentiel de la FFT.
#define TOLERANCE ((float)FE / (1*W))

#define ALPHA 5.0

#define PI 3.14159265358979323846

//
// source .venv/bin/activate
//gcc -Wall -Wextra -O2 main.c yin.c fft.c filter.c -o denoiser -lsndfile -lm

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage : ./denoiser fichier.wav\n");
        return 1;
    }

    /* 1. Lecture du fichier WAV */

    SF_INFO info = {0};

    SNDFILE *input = sf_open(argv[1], SFM_READ, &info);

    if (input == NULL) {
        printf("Erreur ouverture fichier : %s\n",
               sf_strerror(NULL));
        return 1;
    }

    if (info.channels != 1 || info.samplerate != FE ||
        info.frames <= 0) {
        printf("Le fichier doit etre mono, 44100 Hz et non vide.\n");
        sf_close(input);
        return 1;
    }

    size_t nframes = (size_t)info.frames;

    if (nframes > SIZE_MAX - 2 * (W + TAU_MAX)) {
        printf("Fichier trop volumineux.\n");
        sf_close(input);
        return 1;
    }

    /* 2. Allocation mémoire */

    const size_t PAD = W + TAU_MAX;
    const size_t padded_size = nframes + 2 * PAD;

    double *signal = calloc(nframes, sizeof(double));
    double *padded = calloc(padded_size, sizeof(double));

    double *output = calloc(nframes, sizeof(double));
    double *weight_sum = calloc(nframes, sizeof(double));

    if (!signal || !padded || !output || !weight_sum) {
        printf("Erreur allocation memoire.\n");

        free(signal);
        free(padded);
        free(output);
        free(weight_sum);

        sf_close(input);
        return 1;
    }

    sf_count_t read_count =
        sf_readf_double(input, signal, info.frames);

    sf_close(input);

    if (read_count != info.frames) {
        printf("Erreur lecture du fichier WAV.\n");

        free(signal);
        free(padded);
        free(output);
        free(weight_sum);

        return 1;
    }

    // Ajout de zéros avant et après le signal.
    for (size_t i = 0; i < nframes; i++) {
        padded[PAD + i] = signal[i];
    }

    /* 3. Initialisation des tableaux */

    double d[TAU_MAX + 1];
    double d_prime[TAU_MAX + 1];

    double window[W];

    Complex spectrum[W];

    // Fenêtre de Hann.
    for (int i = 0; i < W; i++) {
        window[i] = 0.5 * (
            1.0 - cos(2.0 * PI * i / (W - 1))
        );
    }

    FILE *csv = fopen("yin_results.csv", "w");

    FILE *energy_csv = fopen("energie.csv", "w");

    if (energy_csv == NULL) {
        printf("Erreur creation energie.csv.\n");
        fclose(csv);

        free(signal);
        free(padded);
        free(output);
        free(weight_sum);

        return 1;
    }

    fprintf(energy_csv,
            "temps,tau,f0,alpha,energie_avant,energie_apres,ratio\n");

    if (csv == NULL) {
        printf("Erreur creation CSV.\n");

        free(signal);
        free(padded);
        free(output);
        free(weight_sum);

        return 1;
    }

    fprintf(csv, "temps,tau,f0\n");

    /* 4. Traitement des fenêtres successives */

    size_t nb_frames = 0;
    size_t nb_detected = 0;

    for (size_t debut = 0;
         debut < nframes + PAD;
         debut += H) {

        double temps =
            ((double)debut - PAD) / FE;

        /* 4.1 Estimation de la fondamentale avec YIN */

        difference(
            padded,
            (int)debut,
            W,
            TAU_MAX,
            d
        );

        normalisation(d, TAU_MAX, d_prime);

        int tau = detect_period(
            d_prime,
            TAU_MIN,
            TAU_MAX,
            YIN_THRESHOLD
        );

        double f0 = 0.0;

        if (tau != -1) {
            f0 = (double)FE / tau;
            nb_detected++;
        }

        // On ne conserve dans le CSV que les fenêtres
        // correspondant à des instants du signal original.
        if (temps >= 0.0 && temps < (double)nframes / FE) {
            if (tau == -1) {
                fprintf(csv, "%.5f,-1,0\n", temps);
            } else {
                fprintf(csv, "%.5f,%d,%.2f\n",
                        temps, tau, f0);
            }
        }

        /* 4.2 Préparation de la fenêtre FFT */

        for (int i = 0; i < W; i++) {

            double sample = padded[debut + i];

            spectrum[i].re =
                (float)(sample * window[i]);

            spectrum[i].im = 0.0f;
        }

        /* 4.3 FFT */

        fft(spectrum, W);

        double energie_avant = 0.0;

        for (int k = 0; k < W; k++) {
            energie_avant +=
                (double)spectrum[k].re * spectrum[k].re +
                (double)spectrum[k].im * spectrum[k].im;
        }

        /* 4.4 Filtrage harmonique */

        // Si YIN ne détecte rien, on conserve le spectre.
        if (tau != -1) {

            harmonic_filter(
                spectrum,
                W,
                (float)FE,
                (float)f0,
                TOLERANCE,
                NB_HARMONICS,
                ALPHA
            );
        }

        double energie_apres = 0.0;

        for (int k = 0; k < W; k++) {
            energie_apres +=
                (double)spectrum[k].re * spectrum[k].re +
                (double)spectrum[k].im * spectrum[k].im;
        }

        double ratio = 0.0;

        if (energie_avant > 1e-20) {
            ratio = 100.0 * energie_apres / energie_avant;
        }

        if (temps >= 0.0 && temps < (double)nframes / FE) {

            fprintf(energy_csv,
                    "%.5f,%d,%.2f,%.2f,%.10e,%.10e,%.2f\n",
                    temps,
                    tau,
                    f0,
                    ALPHA,
                    energie_avant,
                    energie_apres,
                    ratio);
        }

        /* 4.5 FFT inverse */

        ifft(spectrum, W);

        /* 4.6 Overlap-Add */

        for (int i = 0; i < W; i++) {

            long long index =
                (long long)debut + i - (long long)PAD;

            if (index >= 0 && (size_t)index < nframes) {

                double w = window[i];

                output[index] +=
                    spectrum[i].re * w;

                weight_sum[index] += w * w;
            }
        }

        nb_frames++;
    }

    fclose(csv);
    fclose(energy_csv);

    /* 5. Normalisation de la reconstruction */

    for (size_t i = 0; i < nframes; i++) {

        if (weight_sum[i] > 1e-12) {
            output[i] /= weight_sum[i];
        } else {
            output[i] = signal[i];
        }
    }

    /* 6. Écriture du fichier WAV */

    SF_INFO out_info = info;

    out_info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    SNDFILE *out = sf_open(
        "under_the_bridge_filtre.wav",
        SFM_WRITE,
        &out_info
    );

    if (out == NULL) {
        printf("Erreur creation WAV : %s\n",
               sf_strerror(NULL));

        free(signal);
        free(padded);
        free(output);
        free(weight_sum);

        return 1;
    }

    sf_count_t written =
        sf_writef_double(out, output, info.frames);

    sf_close(out);

    if (written != info.frames) {
        printf("Erreur ecriture WAV.\n");

        free(signal);
        free(padded);
        free(output);
        free(weight_sum);

        return 1;
    }

    /* 7. Bilan */

    printf("Traitement termine.\n");
    printf("Nombre de fenetres : %zu\n", nb_frames);
    printf("Fondamentale detectee : %zu / %zu\n",
           nb_detected, nb_frames);

    printf("Tolerance : %.2f Hz\n", TOLERANCE);

    printf("Fichier audio : guitare_filtre.wav\n");
    printf("Fichier YIN : yin_results.csv\n");

    free(signal);
    free(padded);
    free(output);
    free(weight_sum);

    return 0;
}
