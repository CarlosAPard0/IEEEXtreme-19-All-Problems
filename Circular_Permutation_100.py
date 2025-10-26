def main():
    n = int(input())
    initial = list(map(int, input().split()))
    
    current_pos = [0] * (n + 1)
    for idx, friend in enumerate(initial):
        current_pos[friend] = idx

    low = 0
    high = n // 2
    ans = high

    def feasible_clockwise(d):
        arr = [0] * (n + 2)
        for h in range(1, n + 1):
            L = (h - 1 - current_pos[h] - d) % n
            R = (h - 1 - current_pos[h] + d) % n
            L = (L + n) % n
            R = (R + n) % n
            if L <= R:
                arr[L] += 1
                if R + 1 < n:
                    arr[R + 1] -= 1
            else:
                arr[0] += 1
                arr[R + 1] -= 1
                arr[L] += 1
        prefix = 0
        for i in range(n):
            prefix += arr[i]
            if prefix == n:
                return True
        return False

    def feasible_anticlockwise(d):
        arr = [0] * (n + 2)
        for h in range(1, n + 1):
            L = (h - 1 + current_pos[h] - d) % n
            R = (h - 1 + current_pos[h] + d) % n
            L = (L + n) % n
            R = (R + n) % n
            if L <= R:
                arr[L] += 1
                if R + 1 < n:
                    arr[R + 1] -= 1
            else:
                arr[0] += 1
                arr[R + 1] -= 1
                arr[L] += 1
        prefix = 0
        for i in range(n):
            prefix += arr[i]
            if prefix == n:
                return True
        return False

    while low <= high:
        mid = (low + high) // 2
        if feasible_clockwise(mid) or feasible_anticlockwise(mid):
            ans = mid
            high = mid - 1
        else:
            low = mid + 1

    print(ans)

if __name__ == '__main__':
    main()