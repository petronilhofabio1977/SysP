#include "dataflow_engine.hpp"
#include <queue>
#include <iostream>

namespace sysp::cfg {

// ── Merge: intersection of initialized sets ───────────────────────
// A variable is definitely initialized only if all predecessors init it
InitState DataflowEngine::merge_init(const InitState& a, const InitState& b) {
    InitState result;
    // Definitely initialized: in both
    for (auto& v : a.initialized)
        if (b.initialized.count(v)) result.initialized.insert(v);
    // Maybe initialized: in either but not both
    for (auto& v : a.initialized)
        if (!b.initialized.count(v)) result.maybe_init.insert(v);
    for (auto& v : b.initialized)
        if (!a.initialized.count(v)) result.maybe_init.insert(v);
    for (auto& v : a.maybe_init) result.maybe_init.insert(v);
    for (auto& v : b.maybe_init) result.maybe_init.insert(v);
    // Declared: union of declared
    for (auto& v : a.declared)   result.declared.insert(v);
    for (auto& v : b.declared)   result.declared.insert(v);
    return result;
}

// ── Transfer: apply block instructions to init state ─────────────
InitState DataflowEngine::transfer_init(
    const InitState& in, const BasicBlock* block)
{
    InitState out = in;
    for (auto& instr : block->instructions) {
        if (instr->opcode == sysp::ir::Opcode::Alloc) {
            // Variable declared — not yet initialized
            out.declare(instr->var_name);
        } else if (instr->opcode == sysp::ir::Opcode::Store) {
            // Variable assigned — now initialized
            out.define(instr->var_name);
        }
    }
    return out;
}

// ── Forward dataflow: initialization analysis ─────────────────────
std::unordered_map<int, InitState>
DataflowEngine::analyze_initialization(
    const std::vector<std::unique_ptr<BasicBlock>>& blocks)
{
    std::unordered_map<int, InitState> in_state;
    std::unordered_map<int, InitState> out_state;

    // Initialize all blocks with empty state
    for (auto& b : blocks) {
        in_state[b->id]  = InitState{};
        out_state[b->id] = InitState{};
    }

    // Worklist algorithm
    std::queue<int> worklist;
    for (auto& b : blocks) worklist.push(b->id);

    // Build id → block map
    std::unordered_map<int, BasicBlock*> block_map;
    for (auto& b : blocks) block_map[b->id] = b.get();

    int iterations = 0;
    while (!worklist.empty() && iterations < 1000) {
        int id = worklist.front(); worklist.pop();
        iterations++;

        auto* block = block_map[id];
        if (!block) continue;

        // IN = merge of all predecessors' OUT
        InitState new_in;
        bool first = true;
        for (auto* pred : block->predecessors) {
            if (first) {
                new_in = out_state[pred->id];
                first  = false;
            } else {
                new_in = merge_init(new_in, out_state[pred->id]);
            }
        }
        // Entry block: start with empty state
        if (block->predecessors.empty()) new_in = InitState{};

        in_state[id] = new_in;

        // OUT = transfer(IN)
        InitState new_out = transfer_init(new_in, block);

        // If OUT changed, re-add successors to worklist
        if (new_out.initialized != out_state[id].initialized ||
            new_out.declared    != out_state[id].declared) {
            out_state[id] = new_out;
            for (auto* succ : block->successors)
                worklist.push(succ->id);
        }
    }

    return in_state;
}

// ── Mark reachability (for dead-code detection) ───────────────────
void DataflowEngine::mark_reachability(
    std::vector<std::unique_ptr<BasicBlock>>& blocks)
{
    if (blocks.empty()) return;

    std::unordered_set<int> visited;
    std::queue<int>         worklist;

    // Start from entry block
    worklist.push(blocks[0]->id);

    std::unordered_map<int, BasicBlock*> block_map;
    for (auto& b : blocks) block_map[b->id] = b.get();

    while (!worklist.empty()) {
        int id = worklist.front(); worklist.pop();
        if (visited.count(id)) continue;
        visited.insert(id);

        auto* block = block_map[id];
        if (!block) continue;

        block->is_reachable = true;
        for (auto* succ : block->successors)
            if (!visited.count(succ->id))
                worklist.push(succ->id);
    }

    // Mark unreachable blocks
    for (auto& b : blocks)
        if (!visited.count(b->id))
            b->is_reachable = false;
}

// ── Backward dataflow: liveness ───────────────────────────────────
std::unordered_map<int, LiveState>
DataflowEngine::analyze_liveness(
    const std::vector<std::unique_ptr<BasicBlock>>& blocks)
{
    std::unordered_map<int, LiveState> in_state;
    std::unordered_map<int, LiveState> out_state;

    for (auto& b : blocks) {
        in_state[b->id]  = LiveState{};
        out_state[b->id] = LiveState{};
    }

    std::unordered_map<int, BasicBlock*> block_map;
    for (auto& b : blocks) block_map[b->id] = b.get();

    std::queue<int> worklist;
    // Process in reverse order for backward analysis
    for (int i = (int)blocks.size() - 1; i >= 0; i--)
        worklist.push(blocks[i]->id);

    int iterations = 0;
    while (!worklist.empty() && iterations < 1000) {
        int id = worklist.front(); worklist.pop();
        iterations++;

        auto* block = block_map[id];
        if (!block) continue;

        // OUT = union of all successors' IN
        LiveState new_out;
        for (auto* succ : block->successors)
            for (auto& v : in_state[succ->id].live)
                new_out.live.insert(v);

        out_state[id] = new_out;

        // IN = USE ∪ (OUT - DEF)
        LiveState new_in;
        // Start with OUT
        new_in = new_out;
        // Remove DEF
        for (auto& [v, _] : block->def) new_in.live.erase(v);
        // Add USE
        for (auto& [v, _] : block->use) new_in.live.insert(v);

        // If IN changed, re-add predecessors
        if (new_in.live != in_state[id].live) {
            in_state[id] = new_in;
            for (auto* pred : block->predecessors)
                worklist.push(pred->id);
        }
    }

    return out_state; // live-out at each block
}

} // namespace sysp::cfg
