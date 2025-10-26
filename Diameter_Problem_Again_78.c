#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define N_MAX 300005
#define LOG 19
#define B 1800
#define INF 1e18

typedef long long ll;

// Structs
typedef struct {
    int v;
    ll w;
} Edge;

typedef struct {
    Edge* data;
    int size;
    int cap;
} Vector;

typedef struct {
    int* data;
    int size;
    int cap;
} IntVector;

typedef struct {
    ll* data;
    int size;
    int cap;
} LLVector;

// Vector operations
void vec_init(Vector* v) {
    v->cap = 10;
    v->size = 0;
    v->data = malloc(v->cap * sizeof(Edge));
}

void vec_push(Vector* v, int u, ll w) {
    if (v->size == v->cap) {
        v->cap *= 2;
        v->data = realloc(v->data, v->cap * sizeof(Edge));
    }
    v->data[v->size].v = u;
    v->data[v->size].w = w;
    v->size++;
}

void intvec_init(IntVector* v) {
    v->cap = 10;
    v->size = 0;
    v->data = malloc(v->cap * sizeof(int));
}

void intvec_push(IntVector* v, int x) {
    if (v->size == v->cap) {
        v->cap *= 2;
        v->data = realloc(v->data, v->cap * sizeof(int));
    }
    v->data[v->size++] = x;
}

void llvec_init(LLVector* v) {
    v->cap = 10;
    v->size = 0;
    v->data = malloc(v->cap * sizeof(ll));
}

void llvec_push(LLVector* v, ll x) {
    if (v->size == v->cap) {
        v->cap *= 2;
        v->data = realloc(v->data, v->cap * sizeof(ll));
    }
    v->data[v->size++] = x;
}

// Global variables
Vector adj[N_MAX];
ll d[N_MAX];
int dep[N_MAX];
IntVector tour, dep_tour;
int first_occ[N_MAX];
int st[LOG][2 * N_MAX];
int tour_sz;
int N;

ll down[N_MAX];
ll f[N_MAX];

// DFS for LCA preprocessing
void dfs_tour(int u, int p, ll dist, int depth) {
    first_occ[u] = tour.size;
    intvec_push(&tour, u);
    intvec_push(&dep_tour, depth);
    d[u] = dist;
    dep[u] = depth;
    
    for (int i = 0; i < adj[u].size; i++) {
        int v = adj[u].data[i].v;
        ll w = adj[u].data[i].w;
        if (v != p) {
            dfs_tour(v, u, dist + w, depth + 1);
            intvec_push(&tour, u);
            intvec_push(&dep_tour, depth);
        }
    }
}

int get_lca(int u, int v) {
    int L = first_occ[u];
    int R = first_occ[v];
    if (L > R) {
        int tmp = L;
        L = R;
        R = tmp;
    }
    int len = R - L + 1;
    int k = 31 - __builtin_clz(len);
    int idx1 = st[k][L];
    int idx2 = st[k][R - (1 << k) + 1];
    int min_idx = (dep_tour.data[idx1] < dep_tour.data[idx2]) ? idx1 : idx2;
    return tour.data[min_idx];
}

ll get_dist(int u, int v) {
    int l = get_lca(u, v);
    return d[u] + d[v] - 2 * d[l];
}

void dfs1(int u, int p, bool* is_target) {
    down[u] = is_target[u] ? 0 : -INF;
    for (int i = 0; i < adj[u].size; i++) {
        int v = adj[u].data[i].v;
        ll w = adj[u].data[i].w;
        if (v != p) {
            dfs1(v, u, is_target);
            down[u] = (down[u] > down[v] + w) ? down[u] : down[v] + w;
        }
    }
}

void dfs2(int u, int p, ll up_from_p, bool* is_target) {
    f[u] = (down[u] > up_from_p) ? down[u] : up_from_p;
    
    ll max1 = -INF, max2 = -INF;
    int child_with_max1 = -1;
    
    for (int i = 0; i < adj[u].size; i++) {
        int v = adj[u].data[i].v;
        ll w = adj[u].data[i].w;
        if (v != p) {
            ll val = down[v] + w;
            if (val > max1) {
                max2 = max1;
                max1 = val;
                child_with_max1 = v;
            } else if (val > max2) {
                max2 = val;
            }
        }
    }
    
    for (int i = 0; i < adj[u].size; i++) {
        int c = adj[u].data[i].v;
        ll w = adj[u].data[i].w;
        if (c != p) {
            ll max_sib = (c == child_with_max1) ? max2 : max1;
            ll max_out = up_from_p;
            if (max_sib > max_out) max_out = max_sib;
            if (is_target[u] && 0 > max_out) max_out = 0;
            
            ll up_for_c = (max_out > -INF / 2) ? (max_out + w) : -INF;
            dfs2(c, u, up_for_c, is_target);
        }
    }
}

int main() {
    scanf("%d", &N);
    
    for (int i = 1; i <= N; i++) {
        vec_init(&adj[i]);
    }
    
    for (int i = 0; i < N - 1; i++) {
        int u, v;
        ll w;
        scanf("%d %d %lld", &u, &v, &w);
        vec_push(&adj[u], v, w);
        vec_push(&adj[v], u, w);
    }
    
    // Precompute LCA
    intvec_init(&tour);
    intvec_init(&dep_tour);
    memset(first_occ, -1, sizeof(first_occ));
    
    dfs_tour(1, -1, 0, 0);
    tour_sz = tour.size;
    
    // Build sparse table
    for (int i = 0; i < tour_sz; i++) {
        st[0][i] = i;
    }
    
    for (int k = 1; k < LOG; k++) {
        for (int i = 0; i + (1 << k) <= tour_sz; i++) {
            int idx1 = st[k - 1][i];
            int idx2 = st[k - 1][i + (1 << (k - 1))];
            st[k][i] = (dep_tour.data[idx1] < dep_tour.data[idx2]) ? idx1 : idx2;
        }
    }
    
    // Query processing
    int M;
    scanf("%d", &M);
    
    LLVector A;
    llvec_init(&A);
    
    LLVector block_fs;
    llvec_init(&block_fs);
    
    int current_fixed = 0;
    ll last_ans = 0;
    
    for (int qi = 0; qi < M; qi++) {
        int type;
        scanf("%d", &type);
        
        if (type == 1) {
            int x_enc;
            scanf("%d", &x_enc);
            int x = ((x_enc ^ (int)abs(last_ans)) % N) + 1;
            llvec_push(&A, x);
            
            if (A.size - current_fixed == B) {
                bool is_target[N_MAX];
                memset(is_target, false, sizeof(is_target));
                
                for (int i = current_fixed; i < A.size; i++) {
                    is_target[(int)A.data[i]] = true;
                }
                
                dfs1(1, -1, is_target);
                dfs2(1, -1, -INF, is_target);
                
                for (int i = 1; i <= N; i++) {
                    llvec_push(&block_fs, f[i]);
                }
                
                current_fixed = A.size;
            }
        } else if (type == 2) {
            if (A.size > 0) {
                A.size--;
                if (A.size < current_fixed) {
                    block_fs.size -= N;
                    current_fixed -= B;
                }
            }
        } else if (type == 3) {
            int l_enc, r_enc, x_enc;
            scanf("%d %d %d", &l_enc, &r_enc, &x_enc);
            
            int sz = A.size;
            if (sz == 0) {
                printf("0\n");
                last_ans = 0;
                continue;
            }
            
            ll abs_last = abs(last_ans);
            int x = ((x_enc ^ (int)abs_last) % N) + 1;
            int l = ((l_enc ^ (int)abs_last) % sz) + 1;
            int r = ((r_enc ^ (int)abs_last) % sz) + 1;
            
            if (l > r) {
                int tmp = l;
                l = r;
                r = tmp;
            }
            
            int left = l - 1, rite = r - 1;
            ll query_ans = -INF;
            
            // Handle fixed part
            int fixed_left = (left > 0) ? left : 0;
            int fixed_rite = (rite < current_fixed - 1) ? rite : current_fixed - 1;
            
            if (fixed_left <= fixed_rite) {
                for (int i = fixed_left; i <= fixed_rite; i++) {
                    ll dist = get_dist(x, (int)A.data[i]);
                    query_ans = (query_ans > dist) ? query_ans : dist;
                }
            }
            
            // Handle suffix part
            int suffix_left = (left > current_fixed) ? left : current_fixed;
            if (suffix_left <= rite) {
                for (int i = suffix_left; i <= rite; i++) {
                    ll dist = get_dist(x, (int)A.data[i]);
                    query_ans = (query_ans > dist) ? query_ans : dist;
                }
            }
            
            printf("%lld\n", query_ans);
            last_ans = query_ans;
        }
    }
    
    return 0;
}