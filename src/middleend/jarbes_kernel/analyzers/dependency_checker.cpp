#include "analyzers.hpp"
#include <unordered_set>
#include <iostream>

// Global set of built-in node IDs — always considered "produced"
std::unordered_set<uint32_t> builtin_nodes;

bool check_use_before_production(const MetatronGraph& graph) {
    // Collect all node IDs that exist in the graph
    std::unordered_set<uint32_t> produced;

    // Built-ins are always produced
    for (auto id : builtin_nodes)
        produced.insert(id);

    bool ok = true;

    for (const auto& node : graph.nodes) {
        // Check all inputs are already produced
        for (auto input_id : node.inputs) {
            if (produced.find(input_id) == produced.end()) {
                std::cerr << "[Jarbes] Error: use before production at node "
                << node.id << " (input " << input_id << " not yet defined)\n";
                ok = false;
            }
        }
        // This node is now produced
        produced.insert(node.id);
    }

    return ok;
}
