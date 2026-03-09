#pragma once

#include "pratt_parser.hpp"
#include "../ast/stmt.hpp"
#include "../ast/ast.hpp"
#include "../../core/token.hpp"

#include <vector>

class Parser {

public:

    Parser(const std::vector<Token>& tokens);

    ASTNodePtr parse();

    Stmt* parse_statement();

private:

    std::vector<Token> tokens;
    size_t pos = 0;
    PrattParser pratt;

};

