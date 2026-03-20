#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "../ir/instruction.hpp"

namespace sysp::cfg {

struct BasicBlock {
    int         id;
    std::string label;
    bool        is_reachable = true;    // for dead-code detection

    std::vector<std::unique_ptr<sysp::ir::Instruction>> instructions;
    std::vector<BasicBlock*> successors;
    std::vector<BasicBlock*> predecessors;

    // Dataflow sets — computed by DataflowEngine
    std::unordered_map<std::string, bool> def;   // vars defined in this block
    std::unordered_map<std::string, bool> use;   // vars used before def
    std::unordered_map<std::string, bool> live_in;
    std::unordered_map<std::string, bool> live_out;
    std::unordered_map<std::string, bool> init_in;   // initialized vars entering
    std::unordered_map<std::string, bool> init_out;  // initialized vars leaving

    explicit BasicBlock(int block_id, const std::string& lbl = "")
        : id(block_id), label(lbl) {}

    sysp::ir::Instruction* add(sysp::ir::Opcode op) {
        instructions.push_back(std::make_unique<sysp::ir::Instruction>(op));
        return instructions.back().get();
    }

    sysp::ir::Instruction* add_const_int(int64_t v, const std::string& name = "") {
        auto* i = add(sysp::ir::Opcode::ConstInt);
        i->const_int   = v;
        i->result_name = name;
        return i;
    }

    sysp::ir::Instruction* add_store(const std::string& var) {
        auto* i = add(sysp::ir::Opcode::Store);
        i->var_name = var;
        def[var]    = true;
        return i;
    }

    sysp::ir::Instruction* add_load(const std::string& var) {
        auto* i = add(sysp::ir::Opcode::Load);
        i->var_name = var;
        if (!def.count(var)) use[var] = true;
        return i;
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
