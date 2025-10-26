// Compilar: gcc -O2 -std=c17 bitonicas.c -o bitonicas
#include <stdio.h>
#include <stdlib.h>

#define MOD 1000000007

static inline int addmod(int a, int b) {
    a += b;
    if (a >= MOD) a -= MOD;
    return a;
}
static inline int submod(int a, int b) {
    a -= b;
    if (a < 0) a += MOD;
    return a;
}

int main(void) {
    int M;
    if (scanf("%d", &M) != 1) return 0;
    if (M <= 0) { puts(""); return 0; }

    // Triangulares t_k = k(k+1)/2, comenzando con tri[0]=0
    int *tri = (int*)malloc((M + 1) * sizeof(int));
    int tri_cnt = 0;
    for (long long k = 0;; ++k) {
        long long t = k * (k + 1) / 2;
        if (t > M) break;
        tri[tri_cnt++] = (int)t;
    }

    // Coeficientes pentagonales gp[n]
    int *gp = (int*)calloc(M + 1, sizeof(int));
    for (long long m = 0;; ++m) {
        long long p1 = m * (3 * m - 1) / 2;
        long long p2 = m * (3 * m + 1) / 2;
        if (p1 > M && p2 > M) break;
        int sgn = (m & 1) ? (MOD - 1) : 1; // mismo signo para p1 y p2
        if (p1 <= M) gp[(int)p1] = sgn;
        if (p2 <= M) gp[(int)p2] = sgn;
    }

    // q[n]
    int *q = (int*)calloc(M + 1, sizeof(int));
    q[0] = 1;
    int kmax = 0;
    for (int n = 1; n <= M; ++n) {
        int s = gp[n];
        while (kmax + 1 < tri_cnt && tri[kmax + 1] <= n) ++kmax;
        for (int idx = 1; idx <= kmax; ++idx) {
            int t = tri[idx];
            int coef = (2 * idx + 1);
            long long term = (long long)q[n - t] * coef % MOD;
            if (idx & 1) s = addmod(s, (int)term);
            else         s = submod(s, (int)term);
        }
        q[n] = s;
    }

    // a[n] = convolución con triangulares alternando signo
    int *a = (int*)calloc(M + 1, sizeof(int));
    for (int idx = 1; idx < tri_cnt; ++idx) {
        int t = tri[idx];
        if (idx & 1) {
            for (int n = t; n <= M; ++n) {
                a[n] = addmod(a[n], q[n - t]);
            }
        } else {
            for (int n = t; n <= M; ++n) {
                a[n] = submod(a[n], q[n - t]);
            }
        }
    }

    for (int n = 1; n <= M; ++n) {
        if (n > 1) putchar(' ');
        printf("%d", a[n]);
    }
    putchar('\n');

    free(tri);
    free(gp);
    free(q);
    free(a);
    return 0;
}