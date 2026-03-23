#include "analyzers.hpp"
#include <iostream>

// Dangling pointer: a pointer/reference to a node that has been freed
// This happens when:
// 1. Node N is in a region R
// 2. Region R closes (N is freed)
// 3. A pointer/reference to N is used after R closes
//
// We detect this by checking if any input to a live node
// is a freed node with a pointer type

bool check_dangling_pointer(const MetatronGraph& graph) {
    bool ok = true;

    for (const auto& node : graph.nodes) {
        // Skip freed nodes themselves
        if (freed_nodes.count(node.id)) continue;

        for (auto input_id : node.inputs) {
            // If input is freed and this node is outside its region
            if (freed_nodes.count(input_id)) {
                int node_reg  = -1;
                int input_reg = -1;
                auto nr = node_region.find(node.id);
                auto ir = node_region.find(input_id);
                if (nr != node_region.end()) node_reg  = nr->second;
                if (ir != node_region.end()) input_reg = ir->second;

                // Dangling: input was in a region but consumer is outside
                if (input_reg >= 0 && node_reg != input_reg) {
                    std::string name;
                    auto nit = node_names.find(input_id);
                    if (nit != node_names.end()) name = " ('" + nit->second + "')";
                    std::cerr << "[Jarbes] Error: dangling-pointer — node "
                              << input_id << name
                              << " points to freed memory (region "
                              << input_reg << " already closed)\n";
                    ok = false;
                }
            }
        }
    }

    if (ok) std::cout << "    [Jarbes] dangling-pointer: OK\n";
    return ok;
}
