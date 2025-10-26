#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXN 300005
#define MAXB 20
#define MAXK 300005

typedef long long ll;

int main() {
    int N, K;
    scanf("%d %d", &N, &K);
    
    ll* A = (ll*)malloc((N + 1) * sizeof(ll));
    for (int i = 1; i <= N; i++) {
        scanf("%lld", &A[i]);
    }
    
    ll* sans = (ll*)calloc(K + 1, sizeof(ll));
    
    // Precompute prefix arrays for each bit
    int* pref = (int*)malloc((N + 1) * sizeof(int));
    
    for (int b = 1; b < MAXB; b++) {
        ll V = 1LL << b;
        
        // Build prefix array for this bit
        memset(pref, 0, (N + 1) * sizeof(int));
        for (int i = 1; i <= N; i++) {
            pref[i] = pref[i - 1] + (A[i] >= V ? 1 : 0);
        }
        
        // Process each query m
        for (int m = 1; m <= K; m++) {
            ll contrib = 0;
            int curr_r = 0;
            
            for (int l = 1; l <= N; l++) {
                int target = m + pref[l - 1];
                
                // Two-pointer technique: move curr_r forward
                while (curr_r < N && (curr_r < l || pref[curr_r] < target)) {
                    curr_r++;
                }
                
                // Check if we found valid position
                if (curr_r > N || pref[curr_r] < target) {
                    continue;
                }
                
                ll rmin = (ll)curr_r;
                ll maxd = N - l + 1;
                ll mind = rmin - l + 1;
                
                if (mind > maxd) {
                    continue;
                }
                
                // Arithmetic progression sum optimization
                ll sum1 = maxd * (maxd + 1) / 2;
                ll sum2 = (mind - 1) * mind / 2;
                contrib += sum1 - sum2;
            }
            
            sans[m] += contrib;
        }
    }
    
    // Output results
    for (int kk = 1; kk <= K; kk++) {
        printf("%lld", sans[kk]);
        if (kk < K) {
            printf(" ");
        } else {
            printf("\n");
        }
    }
    
    // Cleanup
    free(A);
    free(pref);
    free(sans);
    
    return 0;
}