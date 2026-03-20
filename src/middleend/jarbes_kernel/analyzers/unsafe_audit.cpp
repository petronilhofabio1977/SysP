#include "analyzers.hpp"
#include <iostream>

std::unordered_map<uint32_t, bool> unsafe_nodes;

// Unsafe audit:
// - Conta todos os nós marcados como unsafe
// - Reporta como aviso (não bloqueia compilação)
// - Mas se unsafe_count > threshold → aviso extra
// O programador precisa revisar cada bloco unsafe

static const int UNSAFE_WARNING_THRESHOLD = 5;

bool check_unsafe_audit(const MetatronGraph& graph) {
    (void)graph;

    int unsafe_count = 0;
    for (auto& [id, is_unsafe] : unsafe_nodes) {
        if (is_unsafe) unsafe_count++;
    }

    if (unsafe_count == 0) {
        std::cout << "    [Jarbes] unsafe-audit: OK (no unsafe blocks)\n";
        return true;
    }

    if (unsafe_count <= UNSAFE_WARNING_THRESHOLD) {
        std::cout << "    [Jarbes] unsafe-audit: WARNING — "
                  << unsafe_count << " unsafe block(s) — review required\n";
    } else {
        std::cout << "    [Jarbes] unsafe-audit: WARNING — "
                  << unsafe_count << " unsafe blocks — high unsafe usage!\n";
        std::cout << "    Hint: consider refactoring unsafe code into safe abstractions\n";
    }

    // Unsafe is always allowed — just audited
    // To make it an error, uncomment:
    // return false;
    return true;
}
