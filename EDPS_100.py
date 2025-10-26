 import collections

N_str = input()
if not N_str:
    print("No")
else:
    N = int(N_str)

    if N == 0:
        line = input()
        if line.strip() == "":
            print("Yes")
            print("")
        else:
            print("No")
    else:
        A = list(map(int, input().split()))

        if len(A) != N:
            print("No")
        elif any(x % 2 != 0 for x in A):
            print("No")
        elif any(x + 2 > 2 * N for x in A):
            print("No")
        else:
            pairs = []
            for dist in A:
                pairs.append((dist + 2, dist))
            
            pairs.sort(key=lambda x: x[0], reverse=True)
            
            spans = [p[0] for p in pairs]
            dists = [p[1] for p in pairs]
            
            memo = {}

            def solve_recursive(target_len, mask):
                state = (target_len, mask)
                if state in memo:
                    return memo[state]
                
                if target_len == 0:
                    return ("", mask)
                
                if target_len < 0:
                    return (None, mask)

                for i in range(N):
                    if ((mask >> i) & 1) and spans[i] <= target_len:
                        mask_for_inside = mask & ~(1 << i)
                        
                        inside_str, mask_after_inside = solve_recursive(dists[i], mask_for_inside)
                        
                        if inside_str is not None:
                            wps_i = f"({inside_str})"
                            
                            rest_str, final_mask = solve_recursive(target_len - spans[i], mask_after_inside)
                            
                            if rest_str is not None:
                                result = (wps_i + rest_str, final_mask)
                                memo[state] = result
                                return result
                
                memo[state] = (None, mask)
                return (None, mask)

            initial_mask = (1 << N) - 1
            final_str, final_mask = solve_recursive(2 * N, initial_mask)
            
            if final_str is not None and final_mask == 0:
                print("Yes")
                print(final_str)
            else:
                print("No")