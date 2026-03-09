#pragma once

#include <vector>

#include "frontend/ast/expr.hpp"
#include "core/token.hpp"

class PrattParser {

public:

    PrattParser(const std::vector<Token>& tokens);

    Expr* parse_expression(int precedence = 0);

private:

    const std::vector<Token>& tokens;
    size_t pos;

    Token peek();
    Token advance();

    Expr* parse_prefix();
    Expr* parse_infix(Expr* left, Token op);

    int get_precedence(TokenType type);
};
