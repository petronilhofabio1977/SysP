#pragma once
#include <vector>
#include <memory>
#include <string>
#include "opcode.hpp"

// Forward declaration
namespace sysp::cfg { struct BasicBlock; }

namespace sysp::ir {

struct Value;

struct Instruction {
    Opcode              opcode;
    std::vector<Value*> operands;
    std::string         result_name;
    std::string         var_name;
    std::string         type_name;
    std::string         label;
    int64_t             const_int   = 0;
    double              const_float = 0.0;
    std::string         const_str;
    bool                const_bool  = false;
    sysp::cfg::BasicBlock* true_block  = nullptr;
    sysp::cfg::BasicBlock* false_block = nullptr;
    int                 line        = 0;

    explicit Instruction(Opcode op) : opcode(op) {}

    bool is_terminator() const {
        return opcode == Opcode::Jump        ||
               opcode == Opcode::Branch      ||
               opcode == Opcode::Return      ||
               opcode == Opcode::Unreachable;
    }

    bool is_binary() const {
        return opcode == Opcode::Add   || opcode == Opcode::Sub  ||
               opcode == Opcode::Mul   || opcode == Opcode::Div  ||
               opcode == Opcode::Mod   ||
               opcode == Opcode::CmpEQ || opcode == Opcode::CmpNE ||
               opcode == Opcode::CmpLT || opcode == Opcode::CmpLE ||
               opcode == Opcode::CmpGT || opcode == Opcode::CmpGE ||
               opcode == Opcode::And   || opcode == Opcode::Or;
    }

    bool is_const() const {
        return opcode == Opcode::ConstInt   ||
               opcode == Opcode::ConstFloat ||
               opcode == Opcode::ConstStr   ||
               opcode == Opcode::ConstBool;
    }

    bool produces_value() const {
        return opcode != Opcode::Store       &&
               opcode != Opcode::StoreField  &&
               opcode != Opcode::StoreIndex  &&
               opcode != Opcode::Jump        &&
               opcode != Opcode::Branch      &&
               opcode != Opcode::Return      &&
               opcode != Opcode::Unreachable &&
               opcode != Opcode::Nop         &&
               opcode != Opcode::Drop        &&
               opcode != Opcode::Send        &&
               opcode != Opcode::RegionEnter &&
               opcode != Opcode::RegionExit;
    }
};

struct Value {
    Instruction* producer  = nullptr;
    std::string  name;
    std::string  type_name;
    int          id        = -1;
    bool         is_phi    = false;
    bool         is_param  = false;

    explicit Value(Instruction* prod = nullptr) : producer(prod) {}
    bool is_defined() const { return producer != nullptr || is_param; }
};

} // namespace sysp::ir
