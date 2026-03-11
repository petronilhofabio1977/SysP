#pragma once

#include <string>
#include <vector>
#include <memory>

namespace sysp::ast {

struct BlockStmt;

struct Expr {
    virtual ~Expr() = default;
};

struct LiteralExpr : Expr {
    std::string value;
};

struct IdentifierExpr : Expr {
    std::string name;
};

struct BinaryExpr : Expr {

    std::unique_ptr<Expr> left;

    std::string op;

    std::unique_ptr<Expr> right;

};

struct UnaryExpr : Expr {

    std::string op;

    std::unique_ptr<Expr> operand;

};

struct CallExpr : Expr {

    std::unique_ptr<Expr> callee;

    std::vector<std::unique_ptr<Expr>> arguments;

};

struct LambdaExpr : Expr {

    std::vector<std::string> parameters;

    std::unique_ptr<BlockStmt> body;

};

}
