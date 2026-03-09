#pragma once

#include "expr.hpp"
#include <vector>

struct Stmt {
    virtual ~Stmt() = default;
};

struct ExprStmt : Stmt {
    Expr* expr;
};

struct AssignStmt : Stmt {
    std::string name;
    Expr* value;
};

struct ReturnStmt : Stmt {
    Expr* value;
};

struct BlockStmt : Stmt {
    std::vector<Stmt*> statements;
};
