#include "analyzers.hpp"
#include <iostream>

// Buffer overflow: static array access with known out-of-bounds index
// We track array_bounds: node_id → size
// When an array access node has a known constant index,
// we check if index >= bounds

bool check_buffer_overflow(const MetatronGraph& graph) {
    bool ok = true;

    for (const auto& node : graph.nodes) {
        // Check nodes that access arrays (inputs: array_node, index_node)
        if (node.inputs.size() < 2) continue;

        uint32_t array_id = node.inputs[0];
        uint32_t index_id = node.inputs[1];

        // Check if array has known bounds
        auto bounds_it = array_bounds.find(array_id);
        if (bounds_it == array_bounds.end()) continue;

        int bounds = bounds_it->second;

        // Check if index has a known constant value
        // For now we check if index node name contains a numeric literal
        auto idx_name = node_names.find(index_id);
        if (idx_name == node_names.end()) continue;

        // Try to parse as integer
        try {
            int idx = std::stoi(idx_name->second);
            if (idx < 0 || idx >= bounds) {
                std::string arr_name;
                auto an = node_names.find(array_id);
                if (an != node_names.end()) arr_name = " ('" + an->second + "')";
                std::cerr << "[Jarbes] Error: buffer-overflow — array"
                          << arr_name << " accessed at index " << idx
                          << " but size is " << bounds << "\n";
                ok = false;
            }
        } catch (...) {
            // Index is not a constant — cannot check statically
        }
    }

    if (ok) std::cout << "    [Jarbes] buffer-overflow: OK\n";
    return ok;
}
