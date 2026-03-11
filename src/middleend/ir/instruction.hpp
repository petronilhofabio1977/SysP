#pragma once

#include <vector>
#include <memory>

#include "opcode.hpp"

namespace sysp::ir {

struct Value;

struct Instruction {

    Opcode opcode;

    std::vector<Value*> operands;

    Instruction(Opcode op)
        : opcode(op)
    {}

};

struct Value {

    Instruction* producer = nullptr;

};

}
