#pragma once

#include <vector>

#include "../../core/token.hpp"
#include "../ast/expr.hpp"

namespace sysp::parser {

using sysp::ast::Expr;

class PrattParser {

public:

    PrattParser(const std::vector<Token>& tokens);

    Expr* parse_expression(int precedence = 0);

private:

    const std::vector<Token>& tokens;
    size_t current = 0;

    Expr* parse_prefix();
    Expr* parse_infix(Expr* left, Token op);

    Expr* parse_call_expr(Expr* callee);
    Expr* parse_member_expr(Expr* object, const Token& name);

    Token advance();
    Token peek() const;
    Token previous() const;

    int get_precedence(TokenType type) const;
    bool is_at_end() const;

};

}
