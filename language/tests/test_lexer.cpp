// ================================================================
// SysP Lexer Unit Tests
// ================================================================
#include "../../compiler/src/frontend/lexer/lexer.hpp"
#include <iostream>
#include <cassert>
#include <string>

// ── Test helpers ─────────────────────────────────────────────────
static int tests_run = 0;
static int tests_ok  = 0;

#define ASSERT_EQ(a, b) do { \
    tests_run++; \
    if ((a) == (b)) { tests_ok++; } \
    else { std::cerr << "FAIL [" << __LINE__ << "]: " \
                     << #a << " != " << #b << "\n"; } \
} while(0)

#define ASSERT_TRUE(x) do { \
    tests_run++; \
    if (x) { tests_ok++; } \
    else { std::cerr << "FAIL [" << __LINE__ << "]: " #x " is false\n"; } \
} while(0)

// ── Tests ─────────────────────────────────────────────────────────
void test_empty_source() {
    Lexer lex("");
    auto toks = lex.tokenize();
    ASSERT_EQ(toks.size(), 1u);
    ASSERT_EQ(toks[0].type, TokenType::END);
}

void test_keywords() {
    Lexer lex("fn let if else while return");
    auto toks = lex.tokenize();
    ASSERT_EQ(toks[0].type, TokenType::FN);
    ASSERT_EQ(toks[1].type, TokenType::LET);
    ASSERT_EQ(toks[2].type, TokenType::IF);
    ASSERT_EQ(toks[3].type, TokenType::ELSE);
    ASSERT_EQ(toks[4].type, TokenType::WHILE);
    ASSERT_EQ(toks[5].type, TokenType::RETURN);
}

void test_integer_literals() {
    Lexer lex("42  0xFF  0b1010  1_000_000");
    auto toks = lex.tokenize();
    ASSERT_EQ(toks[0].type, TokenType::INTEGER);
    ASSERT_EQ(toks[0].lexeme, "42");
    ASSERT_EQ(toks[1].type, TokenType::INTEGER);
    ASSERT_EQ(toks[1].lexeme, "0xFF");
    ASSERT_EQ(toks[2].type, TokenType::INTEGER);
    ASSERT_EQ(toks[2].lexeme, "0b1010");
    ASSERT_EQ(toks[3].type, TokenType::INTEGER);
    ASSERT_EQ(toks[3].lexeme, "1_000_000");
}

void test_float_literals() {
    Lexer lex("3.14  2.0");
    auto toks = lex.tokenize();
    ASSERT_EQ(toks[0].type, TokenType::FLOAT);
    ASSERT_EQ(toks[1].type, TokenType::FLOAT);
}

void test_string_literal() {
    Lexer lex("\"hello world\"");
    auto toks = lex.tokenize();
    ASSERT_EQ(toks[0].type, TokenType::STRING);
    ASSERT_TRUE(toks[0].lexeme.find("hello world") != std::string::npos);
}

void test_bool_literals() {
    Lexer lex("true false");
    auto toks = lex.tokenize();
    ASSERT_EQ(toks[0].type, TokenType::BOOL_TRUE);
    ASSERT_EQ(toks[1].type, TokenType::BOOL_FALSE);
}

void test_operators() {
    Lexer lex("+ - * / % == != < > <= >= && || = += -= -> ..");
    auto toks = lex.tokenize();
    ASSERT_EQ(toks[0].type, TokenType::PLUS);
    ASSERT_EQ(toks[1].type, TokenType::MINUS);
    ASSERT_EQ(toks[2].type, TokenType::STAR);
    ASSERT_EQ(toks[3].type, TokenType::SLASH);
    ASSERT_EQ(toks[4].type, TokenType::MOD);
}

void test_line_comment() {
    Lexer lex("let x = 1 -- this is a comment\nlet y = 2");
    auto toks = lex.tokenize();
    // should have: let x = 1 let y = 2 END
    ASSERT_EQ(toks[0].type, TokenType::LET);
    ASSERT_EQ(toks[4].type, TokenType::LET);  // after comment
    ASSERT_TRUE(!lex.has_errors());
}

void test_block_comment() {
    Lexer lex("let (* block comment *) x = 1");
    auto toks = lex.tokenize();
    ASSERT_EQ(toks[0].type, TokenType::LET);
    ASSERT_EQ(toks[1].type, TokenType::IDENT);
    ASSERT_EQ(toks[1].lexeme, "x");
    ASSERT_TRUE(!lex.has_errors());
}

void test_identifier() {
    Lexer lex("foo bar_baz _private CamelCase");
    auto toks = lex.tokenize();
    ASSERT_EQ(toks[0].type, TokenType::IDENT);
    ASSERT_EQ(toks[0].lexeme, "foo");
    ASSERT_EQ(toks[1].type, TokenType::IDENT);
    ASSERT_EQ(toks[3].type, TokenType::IDENT);
}

void test_no_errors_on_valid_source() {
    Lexer lex("fn main() {\n  let x: i32 = 42\n  return x\n}");
    lex.tokenize();
    ASSERT_TRUE(!lex.has_errors());
}

void test_line_tracking() {
    Lexer lex("a\nb\nc");
    auto toks = lex.tokenize();
    ASSERT_EQ(toks[0].line, 1);
    ASSERT_EQ(toks[1].line, 2);
    ASSERT_EQ(toks[2].line, 3);
}

void test_type_keywords() {
    Lexer lex("i32 i64 f64 bool string u8");
    auto toks = lex.tokenize();
    ASSERT_EQ(toks[0].type, TokenType::KW_I32);
    ASSERT_EQ(toks[1].type, TokenType::KW_I64);
    ASSERT_EQ(toks[2].type, TokenType::KW_F64);
    ASSERT_EQ(toks[3].type, TokenType::KW_BOOL);
    ASSERT_EQ(toks[4].type, TokenType::KW_STRING);
    ASSERT_EQ(toks[5].type, TokenType::KW_U8);
}

// ── Main ─────────────────────────────────────────────────────────
int main() {
    std::cout << "=== SysP Lexer Tests ===\n";

    test_empty_source();
    test_keywords();
    test_integer_literals();
    test_float_literals();
    test_string_literal();
    test_bool_literals();
    test_operators();
    test_line_comment();
    test_block_comment();
    test_identifier();
    test_no_errors_on_valid_source();
    test_line_tracking();
    test_type_keywords();

    std::cout << "\nResults: " << tests_ok << "/" << tests_run << " passed";
    if (tests_ok == tests_run)
        std::cout << " ✓\n";
    else
        std::cout << " ✗ (" << (tests_run - tests_ok) << " failed)\n";

    return (tests_ok == tests_run) ? 0 : 1;
}
