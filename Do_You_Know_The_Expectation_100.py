def build_basis(A):
    basis = []
    for x in A:
        for b in basis:
            x = min(x, x ^ b)
        if x > 0:
            basis.append(x)
            basis.sort(reverse=True)
    return basis

def get_all_distinct_sums(basis):
    sums = [0]
    for b in basis:
        cur = len(sums)
        for i in range(cur):
            sums.append(sums[i] ^ b)
    return sums

def format_from_fraction(numerator, denominator):
    if denominator == 0:
        return "0.00"
    scaled = numerator * 100
    q, r = divmod(scaled, denominator)
    if r * 2 >= denominator:
        q += 1
    integer_part = q // 100
    frac_part = q % 100
    return f"{integer_part}.{frac_part:02d}"

n = int(input())
k = int(input())
A = list(map(int, input().split()))

basis = build_basis(A)
d = len(basis)

if d == 0:
    print("0.00")
else:
    distinct_sums = get_all_distinct_sums(basis)
    
    total = 0
    for x in distinct_sums:
        total += pow(x, k)
    
    denom = 1 << d
    out = format_from_fraction(total, denom)
    print(out)