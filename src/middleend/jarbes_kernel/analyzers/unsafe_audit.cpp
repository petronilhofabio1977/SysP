#include "analyzers.hpp"
#include <iostream>

std::unordered_map<uint32_t, bool> unsafe_nodes;

// Unsafe audit: report all unsafe blocks found in the program
// This is not an error — it's a visibility feature
// Programmers can see exactly how much unsafe code exists

bool check_unsafe_audit(const MetatronGraph& graph) {
    (void)graph;

    int unsafe_count = 0;
    for (auto& kv : unsafe_nodes) {
        if (kv.second) unsafe_count++;
    }

    if (unsafe_count > 0) {
        std::cout << "    [Jarbes] unsafe-audit: "
                  << unsafe_count << " unsafe block(s) found — review required\n";
    } else {
        std::cout << "    [Jarbes] unsafe-audit: OK (no unsafe blocks)\n";
    }

    return true; // unsafe is allowed, just audited
}
