#include "analyzers.hpp"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <iostream>

static bool dfs(uint32_t node_id,
                const std::unordered_map<uint32_t, std::vector<uint32_t>>& adj,
                std::unordered_set<uint32_t>& visited,
                std::unordered_set<uint32_t>& in_stack)
{
    visited.insert(node_id);
    in_stack.insert(node_id);

    auto it = adj.find(node_id);
    if (it != adj.end()) {
        for (auto neighbor : it->second) {
            if (in_stack.count(neighbor)) {
                std::cerr << "[Jarbes] Error: cycle detected at node " << node_id << "\n";
                return true;
            }
            if (!visited.count(neighbor)) {
                if (dfs(neighbor, adj, visited, in_stack))
                    return true;
            }
        }
    }

    in_stack.erase(node_id);
    return false;
}

bool detect_cycle(const MetatronGraph& graph) {
    std::unordered_map<uint32_t, std::vector<uint32_t>> adj;
    for (const auto& node : graph.nodes)
        for (auto input : node.inputs)
            adj[input].push_back(node.id);

    std::unordered_set<uint32_t> visited;
    std::unordered_set<uint32_t> in_stack;

    for (const auto& node : graph.nodes) {
        if (!visited.count(node.id)) {
            if (dfs(node.id, adj, visited, in_stack))
                return false; // cycle found
        }
    }
    return true; // no cycle
}
