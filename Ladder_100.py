from math import comb

def collision_probability(n):
    sum_a = 0.0
    sum_b = 0.0
    for k in range(n // 2 + 1):
        combos = comb(n - k, k)
        length = n - k
        half_pow = 0.5 ** length
        quarter_pow = 0.25 ** length
        sum_a += combos * half_pow
        sum_b += combos * quarter_pow
    return sum_b / (sum_a * sum_a)

t = int(input())
for _ in range(t):
    s, n = map(int, input().split())
    p = collision_probability(n)
    print(round(19 * s * p))
