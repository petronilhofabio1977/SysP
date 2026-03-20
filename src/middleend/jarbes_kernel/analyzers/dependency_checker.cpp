#include "analyzers.hpp"
#include <unordered_set>
#include <iostream>

std::unordered_set<uint32_t>              builtin_nodes;
std::unordered_map<uint32_t, std::string> node_names;
std::unordered_map<uint32_t, std::string> node_types;
std::unordered_map<uint32_t, int>         array_bounds;

bool check_use_before_production(const MetatronGraph& graph) {
    std::unordered_set<uint32_t> produced;
    for (auto id : builtin_nodes) produced.insert(id);
    bool ok = true;
    for (const auto& node : graph.nodes) {
        for (auto input_id : node.inputs) {
            if (!produced.count(input_id)) {
                std::string name;
                auto it = node_names.find(input_id);
                if (it != node_names.end()) name = " ('" + it->second + "')";
                std::cerr << "[Jarbes] Error: use-before-definition — node "
                          << input_id << name << " used before being defined\n";
                ok = false;
            }
        }
        produced.insert(node.id);
    }
    if (ok) std::cout << "    [Jarbes] use-before-definition: OK\n";
    return ok;
}
