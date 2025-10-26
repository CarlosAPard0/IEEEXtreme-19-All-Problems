from collections import deque

def solve():
    t = int(input().strip())
    outputs = []
    for _ in range(t):
        s = input().strip()
        waiting = [deque() for _ in range(6)]
        case_lines = []
        for idx, ch in enumerate(s, start=1):
            if ch == '1':
                if waiting[4]:
                    seq = waiting[4].popleft()
                    seq.append(idx)
                    waiting[5].append(seq)
                elif waiting[1]:
                    seq = waiting[1].popleft()
                    seq.append(idx)
                    waiting[2].append(seq)
                else:
                    waiting[1].append([idx])
            elif ch == '0':
                seq = waiting[3].popleft()
                seq.append(idx)
                waiting[4].append(seq)
            else:  # ch == '2'
                if waiting[5]:
                    seq = waiting[5].popleft()
                    seq.append(idx)
                    case_lines.append(' '.join(map(str, seq)))
                else:
                    seq = waiting[2].popleft()
                    seq.append(idx)
                    waiting[3].append(seq)
        outputs.extend(case_lines)
    print('\n'.join(outputs))

if __name__ == "__main__":
    solve()