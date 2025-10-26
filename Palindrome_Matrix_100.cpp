#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

// DSU structure
struct DSU {
    vector<int> parent;
    // Map 2D (r, c) to 1D index
    int n, m;
    int to_1d(int r, int c) { return r * m + c; }

    DSU(int rows, int cols) {
        n = rows;
        m = cols;
        parent.resize(n * m);
        for (int i = 0; i < n * m; ++i) {
            parent[i] = i;
        }
    }

    // Find with path compression
    int find(int i) {
        if (parent[i] == i)
            return i;
        return parent[i] = find(parent[i]);
    }
    
    int find(int r, int c) {
        return find(to_1d(r, c));
    }

    // Union by rank (simple version)
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) {
            parent[root_j] = root_i;
        }
    }
    
    void unite(int r1, int c1, int r2, int c2) {
        unite(to_1d(r1, c1), to_1d(r2, c2));
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    cin >> n >> m;
    vector<string> grid(n);
    for (int i = 0; i < n; ++i) {
        cin >> grid[i];
    }

    DSU dsu(n, m);

    // Process rows
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (grid[i][j] == '.') continue;
            // Find the end of the word
            int k = j;
            while (k + 1 < m && grid[i][k + 1] != '.') {
                k++;
            }
            // Link symmetric cells
            int l = j, r = k;
            while (l < r) {
                dsu.unite(i, l, i, r);
                l++;
                r--;
            }
            // Skip to the end of this word
            j = k;
        }
    }

    // Process columns
    for (int j = 0; j < m; ++j) {
        for (int i = 0; i < n; ++i) {
            if (grid[i][j] == '.') continue;
            // Find the end of the word
            int k = i;
            while (k + 1 < n && grid[k + 1][j] != '.') {
                k++;
            }
            // Link symmetric cells
            int u = i, d = k;
            while (u < d) {
                dsu.unite(u, j, d, j);
                u++;
                d--;
            }
            // Skip to the end of this word
            i = k;
        }
    }

    // Group digits by component
    map<int, vector<int>> components;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (grid[i][j] != '.') {
                int root = dsu.find(i, j);
                components[root].push_back(grid[i][j] - '0');
            }
        }
    }

    // Find the optimal digit for each component
    map<int, int> final_digit;
    for (auto& pair : components) {
        int root = pair.first;
        vector<int>& digits = pair.second;
        
        // Sort to find the median
        sort(digits.begin(), digits.end());
        
        // The median V[(k-1)/2] satisfies both cost and tie-breaker
        int median = digits[(digits.size() - 1) / 2];
        final_digit[root] = median;
    }

    // Print the final grid
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (grid[i][j] == '.') {
                cout << '.';
            } else {
                int root = dsu.find(i, j);
                cout << final_digit[root];
            }
        }
        cout << "\n";
    }

    return 0;
}