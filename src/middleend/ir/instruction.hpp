#pragma once
#include <vector>
#include <memory>
#include <string>
#include "opcode.hpp"

// ================================================================
// SysP IR Instructions
// ================================================================

namespace sysp::ir {

struct Value;

// An IR instruction — opcode + operands + optional result name
struct Instruction {
    Opcode              opcode;
    std::vector<Value*> operands;
    std::string         result_name;    // for named values
    int64_t             const_int  = 0; // for ConstInt
    double              const_float = 0; // for ConstFloat
    std::string         const_str;      // for ConstStr

    explicit Instruction(Opcode op) : opcode(op) {}

    bool is_terminator() const {
        return opcode == Opcode::Jump   ||
               opcode == Opcode::Branch ||
               opcode == Opcode::Return;
    }

    bool is_binary() const {
        return opcode == Opcode::Add || opcode == Opcode::Sub ||
               opcode == Opcode::Mul || opcode == Opcode::Div ||
               opcode == Opcode::CmpEQ || opcode == Opcode::CmpNE ||
               opcode == Opcode::CmpLT || opcode == Opcode::CmpLE ||
               opcode == Opcode::CmpGT || opcode == Opcode::CmpGE;
    }
};

// A value produced by an instruction
struct Value {
    Instruction* producer = nullptr;
    std::string  name;
    int          id = -1;

    explicit Value(Instruction* prod = nullptr) : producer(prod) {}
};

} // namespace sysp::ir
