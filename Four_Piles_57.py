MOD = 10**9 + 7
MAX_BIT = 60  # hasta 2^60

def count_zero_xor_in_ranges(L, R):
    # L, R listas de 4 elementos
    # DP[pos][tight_flags] → #tuplas XOR=0 en bits >= pos
    from functools import lru_cache

    @lru_cache(None)
    def dp(pos, flags):
        if pos < 0:
            return 1  # todos los bits procesados, XOR=0

        allowed = []
        next_flags = []

        for i in range(4):
            lo_tight = (flags >> (2*i)) & 1
            hi_tight = (flags >> (2*i + 1)) & 1
            lb = (L[i] >> pos) & 1
            hb = (R[i] >> pos) & 1

            bits = 0
            if lo_tight and hi_tight:
                if lb == 0 and hb == 0:
                    bits = 0b01
                elif lb == 0 and hb == 1:
                    bits = 0b11
                else:
                    bits = 0b10
            elif lo_tight and not hi_tight:
                bits = 0b11 if lb == 0 else 0b10
            elif not lo_tight and hi_tight:
                bits = 0b01 if hb == 0 else 0b11
            else:
                bits = 0b11
            if bits == 0:
                return 0
            allowed.append(bits)

            # flags next bit
            nlo0 = lo_tight & (0 == lb)
            nhi0 = hi_tight & (0 == hb)
            nlo1 = lo_tight & (1 == lb)
            nhi1 = hi_tight & (1 == hb)
            next_flags.append(((nlo0 << (2*i)) | (nhi0 << (2*i+1)),
                               (nlo1 << (2*i)) | (nhi1 << (2*i+1))))

        # Contar combinaciones de paridad XOR=0
        res = 0
        # Enumeramos solo combinaciones de 0/1 en 4 variables que den XOR=0
        for b0 in (0,1) if allowed[0]==0b11 else ((0,) if allowed[0]==0b01 else (1,)):
            for b1 in (0,1) if allowed[1]==0b11 else ((0,) if allowed[1]==0b01 else (1,)):
                for b2 in (0,1) if allowed[2]==0b11 else ((0,) if allowed[2]==0b01 else (1,)):
                    b3 = b0 ^ b1 ^ b2  # fuerza XOR=0
                    if ((allowed[3] >> b3) & 1) == 0:
                        continue
                    nf = 0
                    nf |= next_flags[0][b0]
                    nf |= next_flags[1][b1]
                    nf |= next_flags[2][b2]
                    nf |= next_flags[3][b3]
                    res += dp(pos-1, nf)
                    if res >= MOD:
                        res -= MOD
        return res

    start_flags = 0
    for i in range(4):
        start_flags |= (1 << (2*i)) | (1 << (2*i+1))
    return dp(MAX_BIT, start_flags)

def main():
    q = int(input().strip())
    out = []
    for _ in range(q):
        vals = list(map(int, input().split()))
        A = [vals[0], vals[2], vals[4], vals[6]]
        B = [vals[1], vals[3], vals[5], vals[7]]

        # total tuplas
        total = 1
        ok = True
        for i in range(4):
            if A[i] > B[i]:
                ok = False
                break
            total = (total * (B[i]-A[i]+1)) % MOD

        if not ok:
            out.append("0")
            continue

        zero = count_zero_xor_in_ranges(A, B)
        ans = (total - zero) % MOD
        out.append(str(ans))

    print("\n".join(out))

if __name__ == "__main__":
    main()
