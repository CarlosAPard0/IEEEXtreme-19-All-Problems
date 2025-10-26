#include <bits/stdc++.h>
using namespace std;

const int MOD = 998244353;
const int G = 3;

int mod_pow(long long a, long long e) {
    long long r = 1;
    while (e) {
        if (e & 1) r = r * a % MOD;
        a = a * a % MOD;
        e >>= 1;
    }
    return int(r);
}

int mod_inv(int x) { return mod_pow(x, MOD - 2); }

void ntt(vector<int> &a, bool invert) {
    int n = (int)a.size();
    int lg = 0;
    while ((1 << lg) < n) ++lg;
    vector<int> rev(n);
    for (int i = 1; i < n; ++i) {
        rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (lg - 1));
        if (i < rev[i]) swap(a[i], a[rev[i]]);
    }
    for (int len = 2; len <= n; len <<= 1) {
        int wlen = mod_pow(G, (MOD - 1) / len);
        if (invert) wlen = mod_inv(wlen);
        for (int i = 0; i < n; i += len) {
            long long w = 1;
            for (int j = 0; j < len / 2; ++j) {
                int u = a[i + j];
                int v = int(1LL * a[i + j + len / 2] * w % MOD);
                int x = u + v;
                if (x >= MOD) x -= MOD;
                int y = u - v;
                if (y < 0) y += MOD;
                a[i + j] = x;
                a[i + j + len / 2] = y;
                w = w * wlen % MOD;
            }
        }
    }
    if (invert) {
        int n_inv = mod_inv(n);
        for (int &x : a) x = int(1LL * x * n_inv % MOD);
    }
}

vector<int> multiply(const vector<int> &a, const vector<int> &b) {
    if (a.empty() || b.empty()) return {};
    if ((int)min(a.size(), b.size()) <= 32) {
        vector<int> res(a.size() + b.size() - 1, 0);
        for (size_t i = 0; i < a.size(); ++i)
            for (size_t j = 0; j < b.size(); ++j) {
                res[i + j] = (res[i + j] + int(1LL * a[i] * b[j] % MOD)) % MOD;
            }
        return res;
    }
    int n = 1;
    while (n < (int)a.size() + (int)b.size() - 1) n <<= 1;
    vector<int> fa(a.begin(), a.end()), fb(b.begin(), b.end());
    fa.resize(n);
    fb.resize(n);
    ntt(fa, false);
    ntt(fb, false);
    for (int i = 0; i < n; ++i) fa[i] = int(1LL * fa[i] * fb[i] % MOD);
    ntt(fa, true);
    fa.resize(a.size() + b.size() - 1);
    return fa;
}

vector<int> poly_inv(const vector<int> &f, int n) {
    vector<int> res(1, mod_inv(f[0]));
    while ((int)res.size() < n) {
        int m = min(int(res.size()) << 1, n);
        vector<int> f_cut(min((int)f.size(), m));
        for (size_t i = 0; i < f_cut.size(); ++i) f_cut[i] = f[i];
        vector<int> res_f = multiply(res, f_cut);
        res_f.resize(m);
        for (int i = 0; i < m; ++i) {
            res_f[i] = (i == 0 ? (2 - res_f[i]) % MOD : (MOD - res_f[i]) % MOD);
            if (res_f[i] < 0) res_f[i] += MOD;
        }
        res = multiply(res, res_f);
        res.resize(m);
    }
    res.resize(n);
    return res;
}

vector<int> poly_mod(const vector<int> &f, const vector<int> &g) {
    if (g.size() == 1) return {};
    int n = (int)f.size(), m = (int)g.size();
    if (n < m) return f;
    vector<int> fr(n), gr(m);
    for (int i = 0; i < n; ++i) fr[i] = f[n - 1 - i];
    for (int i = 0; i < m; ++i) gr[i] = g[m - 1 - i];
    vector<int> gr_inv = poly_inv(gr, n - m + 1);
    vector<int> q_rev = multiply(fr, gr_inv);
    q_rev.resize(n - m + 1);
    reverse(q_rev.begin(), q_rev.end());
    vector<int> prod = multiply(g, q_rev);
    vector<int> res(f.begin(), f.end());
    res.resize(max((int)res.size(), (int)prod.size()));
    for (size_t i = 0; i < prod.size(); ++i) {
        res[i] -= prod[i];
        if (res[i] < 0) res[i] += MOD;
    }
    res.resize(m - 1);
    while (!res.empty() && res.back() == 0) res.pop_back();
    return res;
}

vector<int> derivative(const vector<int> &f) {
    if (f.size() <= 1) return {};
    vector<int> res(f.size() - 1);
    for (size_t i = 1; i < f.size(); ++i)
        res[i - 1] = int(1LL * f[i] * i % MOD);
    return res;
}

vector<int> multipoint_eval(const vector<int> &f, const vector<int> &points) {
    int n = points.size();
    if (!n) return {};
    int m = 1;
    while (m < n) m <<= 1;
    vector<vector<int>> tree(2 * m);
    for (int i = 0; i < m; ++i) {
        if (i < n)
            tree[m + i] = { (MOD - points[i]) % MOD, 1 };
        else
            tree[m + i] = { 1 };
    }
    for (int i = m - 1; i > 0; --i) {
        if (tree[i << 1].size() == 1) tree[i] = tree[i << 1 | 1];
        else if (tree[i << 1 | 1].size() == 1) tree[i] = tree[i << 1];
        else tree[i] = multiply(tree[i << 1], tree[i << 1 | 1]);
    }
    vector<vector<int>> rem(2 * m);
    rem[1] = poly_mod(f, tree[1]);
    for (int i = 1; i < m; ++i) {
        if (tree[i].size() == 1) continue;
        if (tree[i << 1].size() > 1)
            rem[i << 1] = poly_mod(rem[i], tree[i << 1]);
        if (tree[i << 1 | 1].size() > 1)
            rem[i << 1 | 1] = poly_mod(rem[i], tree[i << 1 | 1]);
    }
    vector<int> res(n);
    for (int i = 0; i < n; ++i) {
        if (rem[m + i].empty()) res[i] = 0;
        else res[i] = rem[m + i][0] % MOD;
    }
    return res;
}

vector<int> build_poly(const vector<int> &s, int l, int r) {
    if (r - l == 1) return { s[l], 1 };
    int mid = (l + r) >> 1;
    vector<int> left = build_poly(s, l, mid);
    vector<int> right = build_poly(s, mid, r);
    return multiply(left, right);
}

struct State {
    int A, B, C, D;
    int tid;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N, M;
    if (!(cin >> N >> M)) return 0;
    vector<int> S(N);
    long long sum_s = 0;
    for (int i = 0; i < N; ++i) {
        cin >> S[i];
        sum_s = (sum_s + S[i]) % MOD;
    }
    vector<State> states;
    states.reserve(M);
    unordered_map<int, int> t_index;
    vector<int> t_values;
    auto get_tid = [&](int t) {
        auto it = t_index.find(t);
        if (it != t_index.end()) return it->second;
        int idx = (int)t_values.size();
        t_values.push_back(t);
        t_index[t] = idx;
        return idx;
    };

    int A = 1, B = 0, C = 0, D = 1;
    for (int q = 0; q < M; ++q) {
        int type;
        cin >> type;
        if (type == 1) {
            int X; cin >> X;
            A = (A + int(1LL * X * C % MOD)) % MOD;
            B = (B + int(1LL * X * D % MOD)) % MOD;
        } else {
            swap(A, C);
            swap(B, D);
        }
        State cur;
        cur.A = A;
        cur.B = B;
        cur.C = C;
        cur.D = D;
        if (cur.C == 0) cur.tid = -1;
        else {
            int C_inv = mod_inv(cur.C);
            int t = int(1LL * cur.D * C_inv % MOD);
            cur.tid = get_tid(t);
        }
        states.push_back(cur);
    }

    vector<int> S_mod(N);
    for (int i = 0; i < N; ++i) S_mod[i] = (S[i] % MOD + MOD) % MOD;
    vector<int> S_poly;
    if (N) S_poly = build_poly(S_mod, 0, N);

    vector<int> der_poly = derivative(S_poly);
    vector<int> S1_values;
    if (!t_values.empty()) {
        vector<int> eval_Q = multipoint_eval(S_poly, t_values);
        vector<int> eval_Q_prime = multipoint_eval(der_poly, t_values);
        S1_values.resize(t_values.size());
        for (size_t i = 0; i < t_values.size(); ++i) {
            int denom = eval_Q[i];
            int numer = eval_Q_prime[i];
            S1_values[i] = int(1LL * numer * mod_inv(denom) % MOD);
        }
    }

    int N_mod = N % MOD;
    for (const auto &st : states) {
        int res;
        if (st.C == 0) {
            int denom_inv = mod_inv(st.D);
            long long val = ( (1LL * st.A * sum_s + 1LL * st.B * N_mod) % MOD );
            res = int(val * denom_inv % MOD);
        } else {
            int t = t_values[st.tid];
            int S1 = S1_values[st.tid];
            int C_inv = mod_inv(st.C);
            long long term = ( (st.B - 1LL * st.A * t) % MOD + MOD ) % MOD;
            long long val = ( (1LL * st.A * N_mod + term * S1) % MOD );
            res = int(val * C_inv % MOD);
        }
        cout << res << "\n";
    }
    return 0;
}