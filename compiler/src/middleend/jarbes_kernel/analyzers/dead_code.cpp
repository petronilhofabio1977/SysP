#include "analyzers.hpp"
#include "../../cfg/basic_block.hpp"
#include "../../cfg/dataflow_engine.hpp"
#include <iostream>

// ================================================================
// Jarbes Checker: dead-code / unreachable
//
// Detects code that can never be executed:
//   return 1
//   println("nunca executa")  -- dead code
// ================================================================

bool check_dead_code(
    std::vector<std::unique_ptr<sysp::cfg::BasicBlock>>& blocks)
{
    sysp::cfg::DataflowEngine engine;
    engine.mark_reachability(blocks);

    bool ok = true;
    for (auto& block : blocks) {
        if (!block->is_reachable && !block->is_empty()) {
            std::cerr << "[Jarbes] Warning: dead-code — block '"
                      << block->label << "' is unreachable\n";
            // dead-code is a warning, not an error
        }
    }

    std::cout << "    [Jarbes] dead-code: OK\n";
    return ok;
}
