#pragma once

#include <vector>
#include <string>
#include "../../core/token.hpp"

class Lexer {

public:

Lexer(const std::string& source);

std::vector<Token> tokenize();

private:

char advance();
char peek();
char peek_next();

bool is_at_end();

void skip_whitespace();

Token identifier();
Token number();
Token string();

private:

std::string source;

size_t pos = 0;
int line = 1;
int column = 1;

};
