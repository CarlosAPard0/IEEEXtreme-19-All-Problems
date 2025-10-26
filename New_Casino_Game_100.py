mod = 998244353

t = int(input())
for _ in range(t):
    n = int(input())
    if n % 2 == 0:
        print(0)
    else:
        print(pow(n, mod-2, mod))