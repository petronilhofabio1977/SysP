#pragma once

#include <unordered_map>

#include "../../core/token.hpp"
#include "../ast/expr.hpp"

namespace sysp::parser {

using sysp::ast::Expr;

class PrattParser {

public:

    PrattParser();

    Expr* parse_expression(int precedence = 0);

private:

    Expr* parse_prefix();

    Expr* parse_infix(Expr* left, Token op);

};

}
