#pragma once
#include "basic_block.hpp"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <functional>

// ================================================================
// SysP Dataflow Engine
//
// Generic forward/backward dataflow analysis.
// Used by all checkers that need to reason about program flow.
//
// Model:
//   IN[n]  = merge(OUT[pred(n)])
//   OUT[n] = transfer(IN[n], n)
//
// Convergence: iterates until fixpoint (no change in any set)
// ================================================================

namespace sysp::cfg {

// ── Initialization state for variables ───────────────────────────
// Used by uninitialized-use checker
struct InitState {
    std::unordered_set<std::string> initialized;   // definitely initialized
    std::unordered_set<std::string> maybe_init;    // possibly initialized
    std::unordered_set<std::string> declared;      // declared but not yet init

    bool is_init(const std::string& var) const {
        return initialized.count(var) > 0;
    }
    bool is_declared(const std::string& var) const {
        return declared.count(var) > 0 ||
               initialized.count(var) > 0 ||
               maybe_init.count(var) > 0;
    }
    void define(const std::string& var) {
        declared.erase(var);
        maybe_init.erase(var);
        initialized.insert(var);
    }
    void declare(const std::string& var) {
        declared.insert(var);
    }
};

// ── Liveness state ────────────────────────────────────────────────
struct LiveState {
    std::unordered_set<std::string> live;

    bool is_live(const std::string& var) const {
        return live.count(var) > 0;
    }
};

// ── Dataflow Engine ───────────────────────────────────────────────
class DataflowEngine {
public:
    // Forward analysis: initialization tracking
    // Returns map: block_id → InitState at block entry
    std::unordered_map<int, InitState> analyze_initialization(
        const std::vector<std::unique_ptr<BasicBlock>>& blocks);

    // Backward analysis: liveness
    // Returns map: block_id → LiveState at block exit
    std::unordered_map<int, LiveState> analyze_liveness(
        const std::vector<std::unique_ptr<BasicBlock>>& blocks);

    // Mark unreachable blocks (for dead-code detection)
    void mark_reachability(
        std::vector<std::unique_ptr<BasicBlock>>& blocks);

private:
    // Merge two InitStates (intersection = definitely init on all paths)
    InitState merge_init(const InitState& a, const InitState& b);

    // Transfer function: apply block instructions to init state
    InitState transfer_init(const InitState& in, const BasicBlock* block);
};

} // namespace sysp::cfg
