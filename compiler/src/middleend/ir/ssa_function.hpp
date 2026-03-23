#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "instruction.hpp"

// ================================================================
// SysP SSA (Static Single Assignment) Function
// Each variable is assigned exactly once — enables easy analysis
// ================================================================

namespace sysp::ir {

// A basic block in SSA form
struct SSABlock {
    int                                      id;
    std::string                              label;
    std::vector<std::unique_ptr<Instruction>> instructions;
    std::vector<SSABlock*>                   successors;
    std::vector<SSABlock*>                   predecessors;

    explicit SSABlock(int block_id, const std::string& lbl = "")
        : id(block_id), label(lbl) {}

    Instruction* add(Opcode op) {
        instructions.push_back(std::make_unique<Instruction>(op));
        return instructions.back().get();
    }

    bool is_empty() const { return instructions.empty(); }

    Instruction* terminator() const {
        if (instructions.empty()) return nullptr;
        auto* last = instructions.back().get();
        return last->is_terminator() ? last : nullptr;
    }
};

// A function in SSA form
struct SSAFunction {
    std::string                              name;
    std::vector<std::string>                 param_names;
    std::vector<std::unique_ptr<SSABlock>>   blocks;
    SSABlock*                                entry = nullptr;
    std::unordered_map<std::string, Value*>  value_map;
    int                                      next_value_id = 0;

    SSABlock* create_block(const std::string& label = "") {
        int id = (int)blocks.size();
        blocks.push_back(std::make_unique<SSABlock>(id, label));
        if (!entry) entry = blocks.back().get();
        return blocks.back().get();
    }

    void connect(SSABlock* from, SSABlock* to) {
        from->successors.push_back(to);
        to->predecessors.push_back(from);
    }

    Value* new_value(Instruction* producer = nullptr) {
        auto* v = new Value(producer);
        v->id   = next_value_id++;
        return v;
    }
};

} // namespace sysp::ir
