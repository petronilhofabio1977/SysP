#pragma once
#include <string>

enum class TypeKind {

    VOID,

    INT,
    FLOAT,
    BOOL,
    STRING,

    GRAPH,
    TREE,

    HEAP,
    PRIORITY_QUEUE,

    VECTOR,
    MATRIX

};

struct Type {

    TypeKind kind;

    std::string name;

};
