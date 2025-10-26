from collections import defaultdict

PAIRS = [(i, j) for i in range(7) for j in range(i + 1, 7)]
PAIR_PARITY = [(1 << i) ^ (1 << j) for i, j in PAIRS]
PAIR_TO_INDEX = {(i + 1, j + 1): idx for idx, (i, j) in enumerate(PAIRS)}
MERGE_CACHE = {}

def merge_components(comps, u, v):
    key = (comps, u, v)
    cached = MERGE_CACHE.get(key)
    if cached is not None:
        return cached
    bit_u = 1 << u
    bit_v = 1 << v
    idx_u = idx_v = -1
    for idx, mask in enumerate(comps):
        if mask & bit_u:
            idx_u = idx
        if mask & bit_v:
            idx_v = idx
    if idx_u != -1 and idx_u == idx_v:
        MERGE_CACHE[key] = comps
        return comps
    comps_list = list(comps)
    if idx_u == -1 and idx_v == -1:
        comps_list.append(bit_u | bit_v)
    elif idx_u == -1:
        comps_list[idx_v] |= bit_u
    elif idx_v == -1:
        comps_list[idx_u] |= bit_v
    else:
        combined = comps_list[idx_u] | comps_list[idx_v]
        if idx_u > idx_v:
            idx_u, idx_v = idx_v, idx_u
        comps_list[idx_u] = combined
        comps_list.pop(idx_v)
    result = tuple(sorted(comps_list))
    MERGE_CACHE[key] = result
    return result

def count_eulerian_subsets(counts):
    dp = {(0, ()): 1}
    for idx, (u, v) in enumerate(PAIRS):
        c = counts[idx]
        if c == 0:
            continue
        parity_mask = PAIR_PARITY[idx]
        new_dp = defaultdict(int)
        for (parity, comps), ways in dp.items():
            new_dp[(parity, comps)] += ways
            merged = merge_components(comps, u, v)
            shifted = ways << (c - 1)
            new_dp[(parity ^ parity_mask, merged)] += shifted
            if c > 1:
                new_dp[(parity, merged)] += shifted - ways
        dp = new_dp
    total = 0
    for (parity, comps), ways in dp.items():
        if len(comps) != 1:
            continue
        odd = parity.bit_count()
        if odd in (0, 2):
            total += ways
    return total

def main():
    T = int(input())
    outputs = []
    for _ in range(T):
        M = int(input())
        counts = [0] * len(PAIRS)
        for _ in range(M):
            x, y = map(int, input().split())
            counts[PAIR_TO_INDEX[(x, y)]] += 1
        outputs.append(str(count_eulerian_subsets(counts)))
    print("\n".join(outputs))

if __name__ == "__main__":
    main()
