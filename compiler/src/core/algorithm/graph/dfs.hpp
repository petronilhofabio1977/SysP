#pragma once
#include <vector>

namespace sysp::core::algorithm {

static void dfs_impl(
    const std::vector<std::vector<int>>& adj,
    int v,
    std::vector<bool>& visited,
    std::vector<int>&  order)
{
    visited[v] = true;
    order.push_back(v);
    for (int u : adj[v])
        if (!visited[u])
            dfs_impl(adj, u, visited, order);
}

inline std::vector<int> dfs(
    const std::vector<std::vector<int>>& adj,
    int start)
{
    std::vector<bool> visited(adj.size(), false);
    std::vector<int>  order;
    dfs_impl(adj, start, visited, order);
    return order;
}

} // namespace sysp::core::algorithm
