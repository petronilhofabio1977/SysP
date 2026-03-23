#include "analyzers.hpp"
#include <iostream>
#include <unordered_map>

// Tracks constant values of nodes: node_id → integer value
// Populated when a literal integer is assigned to a variable
std::unordered_map<uint32_t, int64_t> node_const_values;

bool check_buffer_overflow(const MetatronGraph& graph) {
    bool ok = true;

    // First pass: collect constant values from literal nodes
    // A node is constant if its name is a pure integer string
    for (const auto& node : graph.nodes) {
        auto nit = node_names.find(node.id);
        if (nit != node_names.end()) {
            try {
                int64_t val = std::stoll(nit->second);
                node_const_values[node.id] = val;
            } catch (...) {}
        }
    }

    // Second pass: check array accesses
    for (const auto& node : graph.nodes) {
        if (node.inputs.size() < 2) continue;

        uint32_t array_id = node.inputs[0];
        uint32_t index_id = node.inputs[1];

        // Check if array has known bounds
        auto bounds_it = array_bounds.find(array_id);
        if (bounds_it == array_bounds.end()) continue;

        int bounds = bounds_it->second;

        // Try to get constant value of index
        int64_t idx = -1;
        bool has_const = false;

        // From literal node name
        auto idx_name = node_names.find(index_id);
        if (idx_name != node_names.end()) {
            try { idx = std::stoll(idx_name->second); has_const = true; }
            catch (...) {}
        }

        // From const values map
        if (!has_const) {
            auto cv = node_const_values.find(index_id);
            if (cv != node_const_values.end()) {
                idx = cv->second;
                has_const = true;
            }
        }

        if (!has_const) continue; // Dynamic index — cannot check statically

        if (idx < 0 || idx >= bounds) {
            std::string arr_name;
            auto an = node_names.find(array_id);
            if (an != node_names.end()) arr_name = " ('" + an->second + "')";
            std::cerr << "[Jarbes] Error: buffer-overflow — array"
                      << arr_name << " accessed at index " << idx
                      << " but declared size is " << bounds << "\n";
            ok = false;
        }
    }

    if (ok) std::cout << "    [Jarbes] buffer-overflow: OK\n";
    return ok;
}
