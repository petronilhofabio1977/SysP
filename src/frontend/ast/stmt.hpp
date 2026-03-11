#pragma once

#include <string>
#include <vector>
#include <memory>

namespace sysp::ast {

struct Expr;

struct Stmt {
    virtual ~Stmt() = default;
};

struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
};

struct ExpressionStmt : Stmt {
    std::unique_ptr<Expr> expression;
};

struct VarDeclStmt : Stmt {

    std::string name;
    std::string type;

    std::unique_ptr<Expr> initializer;

};

struct ConstDeclStmt : Stmt {

    std::string name;
    std::string type;

    std::unique_ptr<Expr> value;

};

struct AssignStmt : Stmt {

    std::string name;
    std::string op;

    std::unique_ptr<Expr> value;

};

struct ReturnStmt : Stmt {
    std::unique_ptr<Expr> value;
};

struct BreakStmt : Stmt {};
struct ContinueStmt : Stmt {};

struct IfStmt : Stmt {

    std::unique_ptr<Expr> condition;

    std::unique_ptr<BlockStmt> then_block;

    std::unique_ptr<BlockStmt> else_block;

};

struct WhileStmt : Stmt {

    std::unique_ptr<Expr> condition;

    std::unique_ptr<BlockStmt> body;

};

struct RangeExpr {

    std::unique_ptr<Expr> start;

    std::unique_ptr<Expr> end;

    bool inclusive = false;

};

struct ForStmt : Stmt {

    std::string iterator;

    RangeExpr range;

    std::unique_ptr<BlockStmt> body;

};

struct Pattern {

    std::string value;

};

struct MatchArm {

    Pattern pattern;

    std::unique_ptr<Stmt> statement;

};

struct MatchStmt : Stmt {

    std::unique_ptr<Expr> value;

    std::vector<MatchArm> arms;

};

}
