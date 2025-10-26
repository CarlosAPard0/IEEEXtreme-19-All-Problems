from collections import deque

def main():
    n = int(input().strip())
    graph = [[] for _ in range(n + 1)]
    
    for _ in range(n - 1):
        u, v = map(int, input().split())
        graph[u].append(v)
        graph[v].append(u)
    
    # Build the tree structure with node 1 as root
    parent = [0] * (n + 1)
    children = [[] for _ in range(n + 1)]
    depth = [0] * (n + 1)
    visited = [False] * (n + 1)
    
    queue = deque([1])
    visited[1] = True
    
    while queue:
        u = queue.popleft()
        for v in graph[u]:
            if not visited[v]:
                visited[v] = True
                parent[v] = u
                children[u].append(v)
                depth[v] = depth[u] + 1
                queue.append(v)
    
    # Process nodes from bottom to top
    nodes = list(range(1, n + 1))
    nodes.sort(key=lambda x: depth[x], reverse=True)
    
    # Assign signatures to subtrees
    signature_id = [0] * (n + 1)
    signature_map = {}
    
    for u in nodes:
        child_signatures = []
        for child in children[u]:
            child_signatures.append(signature_id[child])
        
        # Sort to make signature order-independent
        child_signatures.sort()
        signature_tuple = tuple(child_signatures)
        
        if signature_tuple in signature_map:
            signature_id[u] = signature_map[signature_tuple]
        else:
            new_id = len(signature_map) + 1
            signature_map[signature_tuple] = new_id
            signature_id[u] = new_id
    
    # Count distinguishable nodes
    # Root (node 1) is always distinguishable
    fixed = [False] * (n + 1)
    fixed[1] = True
    count = 1
    
    queue = deque([1])
    while queue:
        u = queue.popleft()
        
        # Group children by their signature
        signature_groups = {}
        for child in children[u]:
            sig = signature_id[child]
            if sig not in signature_groups:
                signature_groups[sig] = []
            signature_groups[sig].append(child)
        
        # Only children with unique signatures are distinguishable
        for sig, child_list in signature_groups.items():
            if len(child_list) == 1:
                child_node = child_list[0]
                fixed[child_node] = True
                count += 1
                queue.append(child_node)
    
    print(count)

if __name__ == '__main__':
    main()