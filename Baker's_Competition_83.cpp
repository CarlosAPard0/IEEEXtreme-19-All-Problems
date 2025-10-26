#include <bits/stdc++.h>
using namespace std;

using ll = long long;

const int MAXN = 1000000;

/**
 * @brief Calcula el factor primo más pequeño para cada número
 * @param smallest_prime_factor Array de SPF de tamaño MAXN+1
 */
void compute_smallest_prime_factors(vector<int>& smallest_prime_factor) {
    for (int i = 0; i <= MAXN; ++i) {
        smallest_prime_factor[i] = i;
    }
    
    for (int i = 2; i * i <= MAXN; ++i) {
        if (smallest_prime_factor[i] == i) {  // i es primo
            for (int multiple = i * i; multiple <= MAXN; multiple += i) {
                if (smallest_prime_factor[multiple] == multiple) {
                    smallest_prime_factor[multiple] = i;
                }
            }
        }
    }
}

/**
 * @brief Identifica todos los números primos hasta MAXN
 * @param smallest_prime_factor Array SPF
 * @param prime_list Vector que almacena todos los primos encontrados
 * @param is_prime Array booleano marcando posiciones primas
 */
void compute_prime_list(const vector<int>& smallest_prime_factor,
                        vector<int>& prime_list,
                        vector<char>& is_prime) {
    is_prime.assign(MAXN + 1, 1);
    is_prime[0] = is_prime[1] = 0;
    
    for (int i = 2; i <= MAXN; ++i) {
        if (smallest_prime_factor[i] == i) {
            is_prime[i] = 1;
            prime_list.push_back(i);
        } else {
            is_prime[i] = 0;
        }
    }
}

/**
 * @brief Construye mapa de índices de primos para búsquedas O(1)
 * @param prime_list Lista de números primos
 * @param prime_to_index Mapa primo -> índice en prime_list
 */
void build_prime_index_map(const vector<int>& prime_list,
                           unordered_map<int, int>& prime_to_index) {
    for (size_t i = 0; i < prime_list.size(); ++i) {
        prime_to_index[prime_list[i]] = i;
    }
}

/**
 * @brief Para cada número k, encuentra todos los primos p tales que k/p es primo
 * @param smallest_prime_factor Array SPF
 * @param is_prime Array de primalidad
 * @param prime_to_index Mapa de índices de primos
 * @param prime_groups Vector donde prime_groups[k] = índices de primos válidos
 */
void compute_prime_groups(const vector<int>& smallest_prime_factor,
                          const vector<char>& is_prime,
                          const unordered_map<int, int>& prime_to_index,
                          vector<vector<int>>& prime_groups) {
    prime_groups.assign(MAXN + 1, vector<int>());
    
    for (int number = 2; number <= MAXN; ++number) {
        set<int> distinct_prime_factors;
        int temp = number;
        
        // Factorizar y obtener factores primos distintos
        while (temp > 1) {
            int prime_factor = smallest_prime_factor[temp];
            distinct_prime_factors.insert(prime_factor);
            temp /= prime_factor;
        }
        
        // Para cada factor primo p, verificar si number/p es primo
        for (int prime : distinct_prime_factors) {
            ll quotient = (ll)number / prime;
            
            if (quotient > 1 && is_prime[quotient]) {
                auto prime_index_it = prime_to_index.find(prime);
                if (prime_index_it != prime_to_index.end()) {
                    prime_groups[number].push_back(prime_index_it->second);
                }
            }
        }
    }
}

/**
 * @struct Query
 * @brief Representa una consulta con rango [l, r] e índice original
 */
struct Query {
    int left_bound;
    int right_bound;
    int original_index;
};

/**
 * @brief Comparador personalizado para Mo's Algorithm
 * Ordena queries por bloques del left_bound, luego por right_bound ascendente
 */
struct MoComparator {
    int block_size;
    
    MoComparator(int bs) : block_size(bs) {}
    
    bool operator()(const Query& query_a, const Query& query_b) const {
        int block_a = query_a.left_bound / block_size;
        int block_b = query_b.left_bound / block_size;
        
        if (block_a != block_b) {
            return block_a < block_b;
        }
        return query_a.right_bound < query_b.right_bound;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    // ========== PREPROCESSING ==========
    vector<int> smallest_prime_factor(MAXN + 1);
    compute_smallest_prime_factors(smallest_prime_factor);
    
    vector<char> is_prime(MAXN + 1);
    vector<int> prime_list;
    compute_prime_list(smallest_prime_factor, prime_list, is_prime);
    
    unordered_map<int, int> prime_to_index;
    build_prime_index_map(prime_list, prime_to_index);
    
    vector<vector<int>> prime_groups(MAXN + 1);
    compute_prime_groups(smallest_prime_factor, is_prime, prime_to_index, prime_groups);
    
    // ========== LECTURA DE QUERIES ==========
    int query_count;
    cin >> query_count;
    
    vector<Query> queries(query_count);
    for (int i = 0; i < query_count; ++i) {
        cin >> queries[i].left_bound >> queries[i].right_bound;
        queries[i].original_index = i;
    }
    
    // ========== MO'S ALGORITHM ==========
    int optimal_block_size = max(1, (int)(MAXN / sqrt(query_count)));
    MoComparator comparator(optimal_block_size);
    sort(queries.begin(), queries.end(), comparator);
    
    vector<ll> query_answers(query_count);
    ll current_contribution = 0;
    int current_left = 1;
    int current_right = 0;
    vector<int> prime_frequency(prime_list.size(), 0);
    
    /**
     * @brief Expande el rango a la derecha agregando elementos
     */
    auto expand_right = [&](int position) {
        for (int prime_index : prime_groups[position]) {
            int previous_count = prime_frequency[prime_index];
            prime_frequency[prime_index]++;
            current_contribution += previous_count;
        }
    };
    
    /**
     * @brief Contrae el rango quitando elementos
     */
    auto shrink_range = [&](int position) {
        for (int prime_index : prime_groups[position]) {
            int previous_count = prime_frequency[prime_index];
            prime_frequency[prime_index]--;
            current_contribution -= (previous_count - 1);
        }
    };
    
    // Procesar cada query
    for (const auto& query : queries) {
        int target_left = query.left_bound;
        int target_right = query.right_bound;
        
        // Expandir a la derecha
        while (current_right < target_right) {
            ++current_right;
            expand_right(current_right);
        }
        
        // Expandir a la izquierda
        while (current_left > target_left) {
            --current_left;
            expand_right(current_left);
        }
        
        // Contraer desde la derecha
        while (current_right > target_right) {
            shrink_range(current_right);
            --current_right;
        }
        
        // Contraer desde la izquierda
        while (current_left < target_left) {
            shrink_range(current_left);
            ++current_left;
        }
        
        query_answers[query.original_index] = current_contribution;
    }
    
    // ========== SALIDA ==========
    for (int i = 0; i < query_count; ++i) {
        cout << query_answers[i] << '\n';
    }
    
    return 0;
}