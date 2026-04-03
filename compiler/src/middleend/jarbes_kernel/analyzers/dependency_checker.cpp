#include "analyzers.hpp"
#include <unordered_set>
#include <iostream>

// ── Global registry definitions (single TU) ───────────────────────
std::unordered_set<uint32_t>              builtin_nodes;
std::unordered_map<uint32_t, bool>        consumed_nodes;
std::unordered_map<uint32_t, int>         node_region;
std::unordered_map<uint32_t, std::string> node_names;
std::unordered_map<uint32_t, bool>        unsafe_nodes;
std::unordered_map<uint32_t, std::string> node_types;
std::unordered_map<uint32_t, int>         array_bounds;
std::unordered_set<uint32_t>              freed_nodes;
std::unordered_map<uint32_t, uint32_t>    node_owner;
std::unordered_set<uint32_t>              spawn_nodes;
std::unordered_set<uint32_t>              move_result_nodes;
std::unordered_set<uint32_t>              channel_nodes;
std::unordered_map<uint32_t, int64_t>     node_const_values;

// ── Reset: call at the start of every analyze() ───────────────────
void reset_jarbes_state() {
    builtin_nodes.clear();
    consumed_nodes.clear();
    node_region.clear();
    node_names.clear();
    unsafe_nodes.clear();
    node_types.clear();
    array_bounds.clear();
    freed_nodes.clear();
    node_owner.clear();
    spawn_nodes.clear();
    move_result_nodes.clear();
    channel_nodes.clear();
    node_const_values.clear();
}

// ── use-before-definition checker ────────────────────────────────
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
