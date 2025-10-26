import bisect

def main():
    import sys
    data = sys.stdin.read().split()
    n = int(data[0])
    q = int(data[1])
    A = list(map(int, data[2:2+n]))
    queries = list(map(int, data[2+n:2+n+q]))
    
    out_lines = []
    for X in queries:
        left_index = bisect.bisect_left(A, X)
        if left_index < n and A[left_index] == X:
            right_index = bisect.bisect_right(A, X) - 1
            out_lines.append(f"{left_index+1} {right_index+1}")
        else:
            out_lines.append("-1 -1")
    
    print("\n".join(out_lines))

if __name__ == "__main__":
    main()