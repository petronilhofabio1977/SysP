#pragma once
#include <vector>
#include <queue>

namespace sysp::core::algorithm {

inline std::vector<int> bfs(
    const std::vector<std::vector<int>>& adj,
    int start)
{
    std::vector<bool> visited(adj.size(), false);
    std::vector<int>  order;
    std::queue<int>   q;

    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        int v = q.front(); q.pop();
        order.push_back(v);
        for (int u : adj[v]) {
            if (!visited[u]) {
                visited[u] = true;
                q.push(u);
            }
        }
    }
    return order;
}

} // namespace sysp::core::algorithm
