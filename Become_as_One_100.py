n_str, q_str = input().split()
n = int(n_str)
q = int(q_str)

a = list(map(int, input().split()))

prefix_sums = [0] * (n + 1)
current_sum = 0
for i in range(n):
    current_sum += (1 << a[i])
    prefix_sums[i + 1] = current_sum

results = []
for _ in range(q):
    l_str, r_str = input().split()
    l = int(l_str)
    r = int(r_str)

    subarray_sum = prefix_sums[r] - prefix_sums[l - 1]

    if subarray_sum > 0 and (subarray_sum & (subarray_sum - 1)) == 0:
        results.append("Yes")
    else:
        results.append("No")

print("\n".join(results))