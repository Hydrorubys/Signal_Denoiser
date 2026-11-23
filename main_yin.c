
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sndfile.h>
#include "yin.h"

#define FE 44100
#define W 1024
#define H 256
#define TAU_MIN 44
#define TAU_MAX 735

//gcc -Wall -Wextra -O2 main_yin.c yin.c -o denoiser -lsndfile -lm

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage : ./denoiser <fichier.wav>\n");
        return 1;
    }

    const char *filename = argv[1];

    SF_INFO info;
    info.format = 0;

    SNDFILE *fichier = sf_open(filename, SFM_READ, &info);

    if (fichier == NULL) {
        printf("Erreur ouverture fichier.\n");
        return 1;
    }

    if (info.channels != 1 || info.samplerate != FE) {
        printf("Format audio incorrect.\n");
        sf_close(fichier);
        return 1;
    }

    double *signal = malloc(info.frames * sizeof(double));
    if (signal == NULL) {
        printf("Erreur allocation.\n");
        sf_close(fichier);
        return 1;
    }

    sf_read_double(fichier, signal, info.frames);

    double d[TAU_MAX + 1];
    double d_prime[TAU_MAX + 1];

    FILE *csv = fopen("yin_results_B.csv", "w");

    if (csv == NULL) {
        printf("Erreur creation CSV.\n");
        free(signal);
        sf_close(fichier);
        return 1;
    }

    fprintf(csv, "temps,tau,f0\n");

    for (int debut = 0;
         debut + W + TAU_MAX < info.frames;
         debut += H) {

        difference(signal, debut, W, TAU_MAX, d);
        normalisation(d, TAU_MAX, d_prime);

        int tau = detect_period(d_prime, TAU_MIN,
                                TAU_MAX, 0.1);

        double temps = (double) debut / FE;

        if (tau == -1) {
            fprintf(csv, "%.4f,-1,0\n", temps);
        } else {
            double f0 = (double) FE / tau;
            fprintf(csv, "%.4f,%d,%.2f\n",
                    temps, tau, f0);
        }
    }

    fclose(csv);
    free(signal);
    sf_close(fichier);

    printf("Analyse terminee : yin_results.csv\n");

    return 0;
}
