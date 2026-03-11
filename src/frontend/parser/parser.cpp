#include "parser.hpp"

namespace sysp::parser {

Parser::Parser()
{
}

std::unique_ptr<sysp::ast::Stmt> Parser::parse_statement()
{
    return parse_expression_stmt();
}

std::unique_ptr<sysp::ast::ExpressionStmt> Parser::parse_expression_stmt()
{
    auto stmt = std::make_unique<sysp::ast::ExpressionStmt>();
    stmt->expression = nullptr;
    return stmt;
}

std::unique_ptr<sysp::ast::VarDeclStmt> Parser::parse_var()
{
    auto var = std::make_unique<sysp::ast::VarDeclStmt>();
    var->name = "var";
    var->type = "i32";
    return var;
}

std::unique_ptr<sysp::ast::ConstDeclStmt> Parser::parse_const()
{
    auto c = std::make_unique<sysp::ast::ConstDeclStmt>();
    c->name = "const";
    c->type = "i32";
    return c;
}

std::unique_ptr<sysp::ast::AssignStmt> Parser::parse_assignment()
{
    auto a = std::make_unique<sysp::ast::AssignStmt>();
    a->name = "var";
    a->op = "=";
    return a;
}

std::unique_ptr<sysp::ast::ReturnStmt> Parser::parse_return()
{
    auto r = std::make_unique<sysp::ast::ReturnStmt>();
    return r;
}

std::unique_ptr<sysp::ast::IfStmt> Parser::parse_if()
{
    auto s = std::make_unique<sysp::ast::IfStmt>();
    s->then_block = std::make_unique<sysp::ast::BlockStmt>();
    return s;
}

std::unique_ptr<sysp::ast::WhileStmt> Parser::parse_while()
{
    auto s = std::make_unique<sysp::ast::WhileStmt>();
    s->body = std::make_unique<sysp::ast::BlockStmt>();
    return s;
}

std::unique_ptr<sysp::ast::ForStmt> Parser::parse_for()
{
    auto s = std::make_unique<sysp::ast::ForStmt>();
    s->iterator = "i";
    s->body = std::make_unique<sysp::ast::BlockStmt>();
    return s;
}

}
