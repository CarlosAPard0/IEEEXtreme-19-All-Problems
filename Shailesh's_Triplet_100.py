import sys

def is_power_of_two(n: int) -> bool:
    return n > 0 and (n & (n - 1)) == 0

def lowbit(x: int) -> int:
    return x & -x

def solve_case(N: int):
    # Imposibles por paridad o si N es potencia de dos
    if (N & 1) or is_power_of_two(N):
        return None

    half = N >> 1
    # Limitar ~N a un ancho seguro para evitar bits infinitos en Python
    mask = (1 << (N.bit_length() + 1)) - 1
    notN = (~N) & mask

    x = half & notN
    y = half & N

    # Construir a, b a partir de x (xor) e y (and)
    if y > 0:
        # Sencillo: ambos > 0 y distintos
        a = y
        b = y | x
    else:
        # y == 0; entonces x tiene al menos dos bits (pues N no es potencia de dos)
        s = lowbit(x)
        a = s
        b = x ^ s

    c = x ^ N

    # Asegurar positividad y distinción (debería cumplirse por construcción)
    if a <= 0 or b <= 0 or c <= 0 or len({a, b, c}) < 3:
        return None

    # Verificación rápida (opcional; comentar en producción si se desea)
    # assert a + b + c == 2 * N
    # assert (a ^ b ^ c) == N

    return a, b, c

def main():
    data = sys.stdin.buffer.read().split()
    t = int(data[0])
    out_lines = []
    idx = 1
    for _ in range(t):
        N = int(data[idx]); idx += 1
        res = solve_case(N)
        if res is None:
            out_lines.append("-1")
        else:
            a, b, c = res
            out_lines.append(f"{a} {b} {c}")
    sys.stdout.write("\n".join(out_lines))

if __name__ == "__main__":
    main()