#pragma once

#include <string>

namespace sysp::ir {

enum class Opcode {
    Nop,

    // constants
    ConstInt,
    ConstFloat,

    // memory
    Load,
    Store,

    // arithmetic
    Add,
    Sub,
    Mul,
    Div,

    // comparisons
    CmpEQ,
    CmpNE,
    CmpLT,
    CmpLE,
    CmpGT,
    CmpGE,

    // control
    Jump,
    Branch,

    // function
    Call,
    Return
};

inline std::string to_string(Opcode op)
{
    switch (op) {
        case Opcode::Nop: return "nop";
        case Opcode::ConstInt: return "const_int";
        case Opcode::ConstFloat: return "const_float";
        case Opcode::Load: return "load";
        case Opcode::Store: return "store";
        case Opcode::Add: return "add";
        case Opcode::Sub: return "sub";
        case Opcode::Mul: return "mul";
        case Opcode::Div: return "div";
        case Opcode::CmpEQ: return "cmpeq";
        case Opcode::CmpNE: return "cmpne";
        case Opcode::CmpLT: return "cmplt";
        case Opcode::CmpLE: return "cmple";
        case Opcode::CmpGT: return "cmpgt";
        case Opcode::CmpGE: return "cmpge";
        case Opcode::Jump: return "jump";
        case Opcode::Branch: return "branch";
        case Opcode::Call: return "call";
        case Opcode::Return: return "return";
    }

    return "unknown";
}

}
