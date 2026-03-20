#include "analyzers.hpp"
#include "../../cfg/ast_to_cfg.hpp"
#include "../../cfg/dataflow_engine.hpp"
#include <iostream>

// ================================================================
// Jarbes Checker: uninitialized-use
// Simulates the dataflow state instruction by instruction
// ================================================================

bool check_uninitialized_use(
    const std::vector<std::unique_ptr<sysp::cfg::BasicBlock>>& blocks)
{
    bool ok = true;

    for (auto& block : blocks) {
        if (!block->is_reachable) continue;

        // Simulate state instruction by instruction within the block
        std::unordered_set<std::string> declared;    // declared, no value yet
        std::unordered_set<std::string> initialized; // has a value

        for (auto& instr : block->instructions) {
            if (instr->opcode == sysp::ir::Opcode::Alloc) {
                // Variable declared — not yet initialized
                declared.insert(instr->var_name);
            }
            else if (instr->opcode == sysp::ir::Opcode::Store) {
                // Variable now has a value
                declared.erase(instr->var_name);
                initialized.insert(instr->var_name);
            }
            else if (instr->opcode == sysp::ir::Opcode::Load) {
                const std::string& var = instr->var_name;
                // If declared but not initialized — error
                if (declared.count(var) && !initialized.count(var)) {
                    std::cerr << "[Jarbes] Error: uninitialized-use — '"
                              << var << "' used before initialization\n";
                    ok = false;
                }
            }
        }
    }

    if (ok) std::cout << "    [Jarbes] uninitialized-use: OK\n";
    return ok;
}
