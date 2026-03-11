#pragma once

#include <vector>
#include <memory>

#include "instruction.hpp"

namespace sysp::ir {

struct BasicBlock;

struct Function {

    std::vector<std::unique_ptr<BasicBlock>> blocks;

};

struct BasicBlock {

    std::vector<std::unique_ptr<Instruction>> instructions;

};

}
