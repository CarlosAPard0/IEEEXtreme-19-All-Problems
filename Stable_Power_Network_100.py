import heapq

def solve():
    t = int(input())
    for _ in range(t):
        n, m = map(int, input().split())
        edges = []
        edges_by_r = []
        for _ in range(m):
            u, v, w, r = map(int, input().split())
            u -= 1
            v -= 1
            edges.append((u, v, w, r))
            edges_by_r.append((r, u, v))

        # DSU to find minimal possible maximum risk R*
        parent = list(range(n))
        size = [1] * n

        def find(x):
            while parent[x] != x:
                parent[x] = parent[parent[x]]
                x = parent[x]
            return x

        def union(a, b):
            ra, rb = find(a), find(b)
            if ra == rb:
                return False
            if size[ra] < size[rb]:
                ra, rb = rb, ra
            parent[rb] = ra
            size[ra] += size[rb]
            return True

        edges_by_r.sort()
        R_star = None

        for r, u, v in edges_by_r:
            union(u, v)
            if find(0) == find(n - 1):
                R_star = r
                break

        if R_star is None:
            print(-1)
            continue

        # Build adjacency for edges with risk <= R_star
        adj = [[] for _ in range(n)]
        for u, v, w, r in edges:
            if r <= R_star:
                adj[u].append((v, w))
                adj[v].append((u, w))

        # Dijkstra
        INF = 10**30
        dist = [INF] * n
        dist[0] = 0
        pq = [(0, 0)]  # (dist, node)

        while pq:
            d, u = heapq.heappop(pq)
            if d != dist[u]:
                continue
            if u == n - 1:
                break
            for v, w in adj[u]:
                nd = d + w
                if nd < dist[v]:
                    dist[v] = nd
                    heapq.heappush(pq, (nd, v))

        if dist[n - 1] >= INF // 2:
            print(-1)
        else:
            print(R_star, dist[n - 1])

if __name__ == "__main__":
    solve()
