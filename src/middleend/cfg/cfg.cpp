#include "basic_block.hpp"

#include <vector>

namespace sysp::cfg {

class ControlFlowGraph {

public:

    std::vector<std::unique_ptr<BasicBlock>> blocks;

    BasicBlock* entry = nullptr;

    BasicBlock* create_block()
    {
        int id = blocks.size();

        blocks.push_back(std::make_unique<BasicBlock>(id));

        if (!entry)
            entry = blocks.back().get();

        return blocks.back().get();
    }

    void connect(BasicBlock* from, BasicBlock* to)
    {
        from->successors.push_back(to);
        to->predecessors.push_back(from);
    }

};

}
