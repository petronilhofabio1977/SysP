#include "analyzers.hpp"
#include "../../cfg/ast_to_cfg.hpp"
#include "../../cfg/dataflow_engine.hpp"
#include "../../../frontend/ast/decl.hpp"
#include <iostream>

// ================================================================
// Jarbes Checker: uninitialized-use
//
// Detects variables declared but used before being assigned.
// Different from use-before-definition:
//
//   use-before-definition: variable doesn't exist at all
//   uninitialized-use:     variable exists but has no value yet
//
// Example:
//   let x: i32       -- declared, not initialized
//   println(x)       -- ERROR: x used before initialization
// ================================================================

bool check_uninitialized_use(
    const std::vector<std::unique_ptr<sysp::cfg::BasicBlock>>& blocks)
{
    sysp::cfg::DataflowEngine engine;
    auto init_states = engine.analyze_initialization(blocks);
    bool ok = true;

    for (auto& block : blocks) {
        if (!block->is_reachable) continue;

        auto& state = init_states[block->id];

        for (auto& instr : block->instructions) {
            // Check Load instructions — these are variable uses
            if (instr->opcode == sysp::ir::Opcode::Load) {
                const std::string& var = instr->var_name;
                if (state.is_declared(var) && !state.is_init(var)) {
                    std::cerr << "[Jarbes] Error: uninitialized-use — '"
                              << var << "' used before initialization\n";
                    ok = false;
                }
                // After use, if uninitialized — don't update state
            }
            // After Store — variable is now initialized
            if (instr->opcode == sysp::ir::Opcode::Store) {
                state.define(instr->var_name);
            }
        }
    }

    if (ok) std::cout << "    [Jarbes] uninitialized-use: OK\n";
    return ok;
}
