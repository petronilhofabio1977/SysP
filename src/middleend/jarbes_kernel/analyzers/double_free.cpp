#include "analyzers.hpp"
#include <iostream>
#include <unordered_map>

std::unordered_set<uint32_t>        freed_nodes;
std::unordered_map<uint32_t,uint32_t> node_owner;

// Double-free happens when:
// 1. A node is freed (region closed or drop called)
// 2. Then freed again
// We track freed_nodes and check for duplicate frees

bool check_double_free(const MetatronGraph& graph) {
    bool ok = true;
    std::unordered_set<uint32_t> already_freed;

    for (const auto& node : graph.nodes) {
        // A node in freed_nodes that appears as input = double-free risk
        if (freed_nodes.count(node.id)) {
            if (already_freed.count(node.id)) {
                std::string name;
                auto nit = node_names.find(node.id);
                if (nit != node_names.end()) name = " ('" + nit->second + "')";
                std::cerr << "[Jarbes] Error: double-free — node "
                          << node.id << name << " freed more than once\n";
                ok = false;
            }
            already_freed.insert(node.id);
        }
    }

    if (ok) std::cout << "    [Jarbes] double-free: OK\n";
    return ok;
}
