def solve():
    line = input().split()
    if not line:
        return False
    
    if len(line) < 3:
        if len(line) == 0:
            return False
        else:
            return False

    N, K = int(line[0]), int(line[1])
    S = line[2]
    
    if not S or len(S) != N:
        return True

    states = [1 if char == 'S' else 0 for char in S]
    
    flips_stop = [0] * (N + 1)
    
    current_flips = 0
    total_ops = 0
    
    for i in range(N):
        current_flips = (current_flips + flips_stop[i]) % 2
        
        effective_state = (states[i] + current_flips) % 2
        
        if effective_state == 1:
            if i + K > N:
                print("-1")
                return True
            
            total_ops += 1
            
            current_flips = (current_flips + 1) % 2
            
            flips_stop[i + K] = (flips_stop[i + K] + 1) % 2
    
    print(total_ops)
    return True

T = int(input())

for _ in range(T):
    if not solve():
        break