import collections

def main():
    N, Q = map(int, input().split())
    colors = list(map(int, input().split()))
    
    graph = [[] for _ in range(N+1)]
    for _ in range(N-1):
        a, b = map(int, input().split())
        graph[a].append(b)
        graph[b].append(a)
        
    dist_white = [10**9] * (N+1)
    q = collections.deque()
    for i in range(1, N+1):
        if colors[i-1] == 1:
            dist_white[i] = 0
            q.append(i)
            
    while q:
        u = q.popleft()
        for v in graph[u]:
            if dist_white[v] > dist_white[u] + 1:
                dist_white[v] = dist_white[u] + 1
                q.append(v)
                
    LOG = 20
    depth = [0] * (N+1)
    parent = [[0] * LOG for _ in range(N+1)]
    visited = [False] * (N+1)
    q.append(1)
    visited[1] = True
    depth[1] = 0
    while q:
        u = q.popleft()
        for v in graph[u]:
            if not visited[v]:
                visited[v] = True
                depth[v] = depth[u] + 1
                parent[v][0] = u
                q.append(v)
                
    min_val = [[10**9] * LOG for _ in range(N+1)]
    for i in range(1, N+1):
        if i == 1:
            min_val[i][0] = dist_white[i]
        else:
            min_val[i][0] = min(dist_white[i], dist_white[parent[i][0]])
            
    for j in range(1, LOG):
        for i in range(1, N+1):
            p = parent[i][j-1]
            if p:
                parent[i][j] = parent[p][j-1]
                min_val[i][j] = min(min_val[i][j-1], min_val[p][j-1])
            else:
                parent[i][j] = 0
                min_val[i][j] = min_val[i][j-1]
                
    def lca(a, b):
        if depth[a] < depth[b]:
            a, b = b, a
        k = depth[a] - depth[b]
        for j in range(LOG-1, -1, -1):
            if k >= (1 << j):
                a = parent[a][j]
                k -= (1 << j)
        if a == b:
            return a
        for j in range(LOG-1, -1, -1):
            if parent[a][j] != parent[b][j]:
                a = parent[a][j]
                b = parent[b][j]
        return parent[a][0]
    
    def get_min(A, B):
        res = dist_white[A]
        k = depth[A] - depth[B]
        for j in range(LOG-1, -1, -1):
            if k >= (1 << j):
                res = min(res, min_val[A][j])
                A = parent[A][j]
                k -= (1 << j)
        return res
        
    out_lines = []
    for _ in range(Q):
        U, V = map(int, input().split())
        L = lca(U, V)
        ans1 = get_min(U, L)
        ans2 = get_min(V, L)
        out_lines.append(str(min(ans1, ans2)))
        
    print("\n".join(out_lines))
    
if __name__ == "__main__":
    main()
