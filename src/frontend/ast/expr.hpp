#pragma once

#include <string>
#include <vector>

struct Expr {
    virtual ~Expr() = default;
};

struct LiteralExpr : Expr {

    std::string value;
};

struct IdentifierExpr : Expr {

    std::string name;
};

struct AssignExpr : Expr {

    std::string name;
    Expr* value;
};

struct BinaryExpr : Expr {

    std::string op;

    Expr* left;
    Expr* right;
};

struct UnaryExpr : Expr {

    std::string op;

    Expr* expr;
};

struct CallExpr : Expr {

    std::string callee;

    std::vector<Expr*> args;
};

