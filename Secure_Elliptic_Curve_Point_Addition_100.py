def mod_inverse(a, p):
    return pow(a, p - 2, p)

def elliptic_curve_add(a, b, p, x1, y1, x2, y2):
    if x1 == x2:
        if (y1 + y2) % p == 0:
            return "POINT_AT_INFINITY"
        numerator = (3 * x1 * x1 + a) % p
        denominator = (2 * y1) % p
        slope = (numerator * mod_inverse(denominator, p)) % p
    else:
        numerator = (y2 - y1) % p
        denominator = (x2 - x1) % p
        slope = (numerator * mod_inverse(denominator, p)) % p
    
    x3 = (slope * slope - x1 - x2) % p
    y3 = (slope * (x1 - x3) - y1) % p
    
    return f"{x3} {y3}"

T = int(input())

for _ in range(T):
    a, b, p, x1, y1, x2, y2 = map(int, input().split())
    result = elliptic_curve_add(a, b, p, x1, y1, x2, y2)
    print(result)