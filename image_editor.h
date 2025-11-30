#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef struct rgb_t {
    int r, g, b;
} rgb_t;

void LOAD(rgb_t ***rgb, char s[100], char f[5], int *n, int *m,
         int *I, int ***mat, int *fl, int *x1, int *x2, int *y1, int *y2);
void free_mat(int ***mat, int *n);
void free_rgb(rgb_t ***rgb, int *n);
void SAVE(rgb_t ***rgb, int ***mat, int *n, int *m, char s[100], char f[5],
        int *I);
void SELECT(char s[100], int *x1, int *x2, int *y1, int *y2, int *n, int *m);
void CROP(rgb_t ***rgb, int ***mat, int *n, int *m, int *x1, int *x2,
         int *y1, int *y2, char f[5]);
void APPLY(rgb_t ***rgb, int *n, int *m, char f[5], char s[100], int *x1,
         int *x2, int *y1, int *y2);
void EQUALIZE(int ***mat, int *n, int *m, char f[5]);
void HISTOGRAM(int ***mat, int *n, int *m, char s[100], char f[5]);
void ROTATE(rgb_t ***rgb, int ***mat, int *n, int *m, char f[5], int *angle,
            int *x1, int *x2, int *y1, int *y2);
