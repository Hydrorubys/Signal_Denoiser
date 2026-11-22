#include <math.h>

void difference(
    const double *signal,
    int debut,
    int W,
    int tau_max,
    double *d
) {
    for (int tau = 0; tau <= tau_max; tau++) {
        d[tau] = 0.0;

        for (int j = 0; j < W; j++) {
            double ecart = signal[debut + j]
                         - signal[debut + j + tau];

            d[tau] += ecart * ecart;
        }
    }
}

void normalisation(
    const double *d,
    int tau_max,
    double *d_prime
) {
    d_prime[0] = 1.0;

    double somme = 0.0;

    for (int tau = 1; tau <= tau_max; tau++) {
        somme += d[tau];

        if (somme == 0.0) {
            d_prime[tau] = 1.0;
        } else {
            d_prime[tau] = d[tau] * tau / somme;
        }
    }
}


int detect_period(
    const double *d_prime,
    int tau_min,
    int tau_max,
    double seuil
) {
    for (int tau = tau_min; tau < tau_max; tau++) {

        if (d_prime[tau] < seuil &&
            d_prime[tau] < d_prime[tau + 1]) {

            // On descend jusqu'au minimum local
            while (tau + 1 < tau_max &&
                   d_prime[tau + 1] < d_prime[tau]) {
                tau++;
            }

            return tau;
        }
    }

    return -1; // Aucune période détectée
}
