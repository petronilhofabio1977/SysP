#pragma once
#include <vector>
#include <memory>
#include <string>
#include "../ir/instruction.hpp"

// ================================================================
// SysP Control Flow Graph — Basic Block
// ================================================================

namespace sysp::cfg {

struct BasicBlock {
    int         id;
    std::string label;

    std::vector<std::unique_ptr<sysp::ir::Instruction>> instructions;
    std::vector<BasicBlock*> successors;
    std::vector<BasicBlock*> predecessors;

    explicit BasicBlock(int block_id, const std::string& lbl = "")
        : id(block_id), label(lbl) {}

    sysp::ir::Instruction* add(sysp::ir::Opcode op) {
        instructions.push_back(std::make_unique<sysp::ir::Instruction>(op));
        return instructions.back().get();
    }

    bool is_entry() const { return predecessors.empty(); }
    bool is_exit()  const { return successors.empty(); }
    bool is_empty() const { return instructions.empty(); }

    sysp::ir::Instruction* terminator() const {
        if (instructions.empty()) return nullptr;
        auto* last = instructions.back().get();
        return last->is_terminator() ? last : nullptr;
    }
};

} // namespace sysp::cfg
