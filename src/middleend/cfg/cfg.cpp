#include "basic_block.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

// ================================================================
// SysP Control Flow Graph
// Built from the AST after Jarbes analysis
// Used by the optimizer and backend
// ================================================================

namespace sysp::cfg {

class ControlFlowGraph {
public:
    std::vector<std::unique_ptr<BasicBlock>> blocks;
    BasicBlock* entry = nullptr;
    BasicBlock* exit  = nullptr;

    BasicBlock* create_block(const std::string& label = "") {
        int id = (int)blocks.size();
        blocks.push_back(std::make_unique<BasicBlock>(id, label));
        if (!entry) entry = blocks.back().get();
        return blocks.back().get();
    }

    void connect(BasicBlock* from, BasicBlock* to) {
        from->successors.push_back(to);
        to->predecessors.push_back(from);
    }

    // Find block by label
    BasicBlock* find(const std::string& label) const {
        for (auto& b : blocks)
            if (b->label == label) return b.get();
        return nullptr;
    }

    int block_count()       const { return (int)blocks.size(); }
    int instruction_count() const {
        int total = 0;
        for (auto& b : blocks) total += (int)b->instructions.size();
        return total;
    }
};

} // namespace sysp::cfg
