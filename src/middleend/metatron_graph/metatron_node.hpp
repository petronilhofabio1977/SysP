#pragma once

#include <vector>
#include <cstdint>

using NodeId = uint32_t;

enum class NodeKind {

    Const,
    Add,
    Sub,
    Mul,
    Div,

    Load,
    Store,

    Call,

    Return,

    Branch,
    Jump,

    Phi
};

struct Node {

    NodeId id;

    NodeKind kind;

    std::vector<NodeId> inputs;

    std::vector<NodeId> users;
};
