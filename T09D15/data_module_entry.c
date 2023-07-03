#include <stdio.h>
#include <stdlib.h>

#include "data_process.h"

void input(double *a, int n);
void output(double *a, int n);

int main() {
    int n;
    scanf("%d", &n);
    // Don`t forget to allocate memory !
    double *data = calloc(n, sizeof(double));
    input(data, n);
    if (normalization(data, n) == 1) {
        output(data, n);
        free(data);
    } else {
        printf("ERROR");
        free(data);
    }
}
void input(double *a, int n) {
    for (double *i = a; i - a < n; i++) {
        scanf("%le", i);
    }
}
void output(double *a, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d : %f", i + 1, a[i]);
        if (i < n - 1) {
            printf("\n");
        } else {
            printf("\n");
        }
    }
}