#ifndef YIN_H
#define YIN_H

void difference(const double *signal, int debut, int W,
                int tau_max, double *d);

void normalisation(const double *d, int tau_max,
                   double *d_prime);

int detect_period(const double *d_prime, int tau_min,
                  int tau_max, double seuil);

#endif
