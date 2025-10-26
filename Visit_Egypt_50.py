MOD = 1_000_000_007
STEP = 212  # 5300 / 25
COINS = [1, 2, 4, 20, 40, 80, 200, 400, 800]

def precompute(max_n):
    if max_n == 0:
        return [1]
    max_amount = STEP * max_n
    size = 1024  # >= max coin; power of two for fast masking
    mask = size - 1
    buffer = [0] * size
    buffer[0] = 1  # dp[0]
    ways = [0] * (max_n + 1)
    ways[0] = 1
    coins = COINS
    mod = MOD
    step = STEP

    for current in range(1, max_amount + 1):
        idx = current & mask
        total = 0
        for coin in coins:
            if coin > current:
                break
            total += buffer[(idx - coin) & mask]
            if total >= mod:
                total -= mod
        buffer[idx] = total
        if current % step == 0:
            ways[current // step] = total
    return ways

def solve():
    T = int(input())
    cases = []
    max_n = 0
    for _ in range(T):
        N, M, A = map(int, input().split())
        cases.append((N, M, A))
        if N > max_n:
            max_n = N

    ways = precompute(max_n)

    for N, M, A in cases:
        actual = ways[N] % MOD
        dist_m = abs(M - actual)
        dist_a = abs(A - actual)

        if M == actual and A == actual:
            print("TIE")
        elif M == actual:
            print("Mikel")
        elif A == actual:
            print("Andrew")
        elif dist_m < dist_a:
            print("Mikel")
        elif dist_a < dist_m:
            print("Andrew")
        else:
            print("NONE")

solve()