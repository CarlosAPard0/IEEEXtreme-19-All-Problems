def solve():
    n, m = map(int, input().split())
    matrix = []
    for _ in range(n):
        row = list(map(int, input().split()))
        matrix.append(row)
    
    # Memoization cache: dp[i][j] = longest path starting from (i, j)
    dp = [[-1] * m for _ in range(n)]
    
    def dfs(i, j):
        # If already computed, return cached result
        if dp[i][j] != -1:
            return dp[i][j]
        
        # Base case: path of length 1 (just current cell)
        max_length = 1
        
        # Try all 4 directions: up, down, left, right
        directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
        
        for di, dj in directions:
            ni, nj = i + di, j + dj
            
            # Check bounds
            if 0 <= ni < n and 0 <= nj < m:
                # Only move if next cell has strictly greater value
                if matrix[ni][nj] > matrix[i][j]:
                    max_length = max(max_length, 1 + dfs(ni, nj))
        
        dp[i][j] = max_length
        return max_length
    
    # Try starting from every cell and find the maximum
    result = 0
    for i in range(n):
        for j in range(m):
            result = max(result, dfs(i, j))
    
    print(result)

solve()