def solve():
    n, k = map(int, input().split())

    costs = [0] * n
    unique_costs = set()
    
    if n > 1:
        input_costs = list(map(int, input().split()))
        for i in range(n - 1):
            city_cost = input_costs[i]
            costs[i + 1] = city_cost
            unique_costs.add(city_cost)

    dp = [False] * (k + 1)
    dp[0] = True

    for c in unique_costs:
        loop_cost = 2 * c
        if 0 < loop_cost <= k:
            for i in range(loop_cost, k + 1):
                if dp[i - loop_cost]:
                    dp[i] = True

    q = int(input())

    results = []
    
    for _ in range(q):
        a, b = map(int, input().split())

        base_cost = costs[a] + costs[b]
        loop_cost_needed = k - base_cost

        if loop_cost_needed >= 0 and dp[loop_cost_needed]:
            results.append("Yes")
        else:
            results.append("No")

    print("\n".join(results))

solve()