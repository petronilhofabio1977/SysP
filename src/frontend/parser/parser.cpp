#include "parser.hpp"
#include "pratt_parser.hpp"

Parser::Parser(const std::vector<Token>& tokens)
: tokens(tokens), pos(0), pratt(tokens) {}

ASTNodePtr Parser::parse()
{
    auto block = new BlockStmt();

    block->statements.push_back(parse_statement());

    return block;
}

Stmt* Parser::parse_statement()
{
    Expr* expr = pratt.parse_expression(0);

    auto* stmt = new ExprStmt();
    stmt->expr = expr;

    return stmt;
}

