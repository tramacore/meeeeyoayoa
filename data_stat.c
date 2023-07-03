#include "data_stat.h"

#include <math.h>

double max(double *data, int n) {
    double with = data[0];
    for (int i = 0; i < n; i++) {
        if (with < data[i]) {
            with = data[i];
        }
    }
    return with;
}

double min(double *data, int n) {
    double with = data[n - 1];
    for (int i = 0; i < n; i++) {
        if (with > data[i]) {
            with = data[i];
        }
    }
    return with;
}
double mean(double *data, int n) {
    double meann = 0;
    for (int i = 0; i < n; i++) {
        meann = meann + data[i];
    }
    return meann;
}
double variance(double *data, int n) {
    double var;
    for (int i = 0; i < n; i++) {
        var = var + (data[i] - mean(data, n)) * (data[i] - mean(data, n));
    }
    var = var / (n - 1);
    return var;
}