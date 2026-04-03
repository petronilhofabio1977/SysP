// ================================================================
// SysP Parser Unit Tests
// ================================================================
#include "../../compiler/src/frontend/lexer/lexer.hpp"
#include "../../compiler/src/frontend/parser/parser.hpp"
#include "../../compiler/src/frontend/ast/decl.hpp"
#include <iostream>
#include <cassert>

static int tests_run = 0;
static int tests_ok  = 0;

#define ASSERT_EQ(a, b) do { \
    tests_run++; \
    if ((a) == (b)) { tests_ok++; } \
    else { std::cerr << "FAIL [" << __LINE__ << "]: " \
                     << #a << " expected " << (b) << "\n"; } \
} while(0)

#define ASSERT_TRUE(x) do { \
    tests_run++; \
    if (x) { tests_ok++; } \
    else { std::cerr << "FAIL [" << __LINE__ << "]: " #x " is false\n"; } \
} while(0)

#define ASSERT_NO_THROW(code) do { \
    tests_run++; \
    try { code; tests_ok++; } \
    catch (const std::exception& e) { \
        std::cerr << "FAIL [" << __LINE__ << "]: threw: " << e.what() << "\n"; } \
} while(0)

static sysp::ast::Program parse(const std::string& src) {
    Lexer lex(src);
    auto tokens = lex.tokenize();
    sysp::parser::Parser p(std::move(tokens));
    return p.parse_program();
}

// ── Tests ─────────────────────────────────────────────────────────
void test_empty_module() {
    ASSERT_NO_THROW(parse("module main\n"));
}

void test_simple_function() {
    auto prog = parse(
        "module main\n"
        "fn add(a i32, b i32) -> i32 {\n"
        "    return a\n"
        "}\n"
    );
    ASSERT_EQ(prog.declarations.size(), 1u);
    auto* fn = dynamic_cast<sysp::ast::FunctionDecl*>(prog.declarations[0].get());
    ASSERT_TRUE(fn != nullptr);
    ASSERT_EQ(fn->name, "add");
    ASSERT_EQ(fn->parameters.size(), 2u);
    ASSERT_EQ(fn->return_type, "i32");
}

void test_let_declaration() {
    ASSERT_NO_THROW(parse(
        "module main\n"
        "fn main() {\n"
        "    let x = 42\n"
        "}\n"
    ));
}

void test_if_else() {
    ASSERT_NO_THROW(parse(
        "module main\n"
        "fn test(x i32) -> i32 {\n"
        "    if x > 0 {\n"
        "        return x\n"
        "    } else {\n"
        "        return 0\n"
        "    }\n"
        "}\n"
    ));
}

void test_while_loop() {
    ASSERT_NO_THROW(parse(
        "module main\n"
        "fn countdown(n i32) {\n"
        "    while n > 0 {\n"
        "        n -= 1\n"
        "    }\n"
        "}\n"
    ));
}

void test_struct_declaration() {
    auto prog = parse(
        "module main\n"
        "struct Point {\n"
        "    x i32\n"
        "    y i32\n"
        "}\n"
    );
    ASSERT_EQ(prog.declarations.size(), 1u);
    auto* s = dynamic_cast<sysp::ast::StructDecl*>(prog.declarations[0].get());
    ASSERT_TRUE(s != nullptr);
    ASSERT_EQ(s->name, "Point");
    ASSERT_EQ(s->fields.size(), 2u);
}

void test_enum_declaration() {
    ASSERT_NO_THROW(parse(
        "module main\n"
        "enum Cor { Vermelho, Verde, Azul }\n"
    ));
}

void test_pub_function() {
    auto prog = parse(
        "module main\n"
        "pub fn soma(a i32, b i32) -> i32 {\n"
        "    return a\n"
        "}\n"
    );
    auto* fn = dynamic_cast<sysp::ast::FunctionDecl*>(prog.declarations[0].get());
    ASSERT_TRUE(fn != nullptr);
    ASSERT_TRUE(fn->is_pub);
}

void test_multiple_functions() {
    auto prog = parse(
        "module main\n"
        "fn foo() {}\n"
        "fn bar() {}\n"
        "fn baz() {}\n"
    );
    ASSERT_EQ(prog.declarations.size(), 3u);
}

void test_const_declaration() {
    ASSERT_NO_THROW(parse(
        "module main\n"
        "const MAX: i32 = 100\n"
    ));
}

void test_return_no_value() {
    ASSERT_NO_THROW(parse(
        "module main\n"
        "fn nothing() {\n"
        "    return\n"
        "}\n"
    ));
}

// ── Main ─────────────────────────────────────────────────────────
int main() {
    std::cout << "=== SysP Parser Tests ===\n";

    test_empty_module();
    test_simple_function();
    test_let_declaration();
    test_if_else();
    test_while_loop();
    test_struct_declaration();
    test_enum_declaration();
    test_pub_function();
    test_multiple_functions();
    test_const_declaration();
    test_return_no_value();

    std::cout << "\nResults: " << tests_ok << "/" << tests_run << " passed";
    if (tests_ok == tests_run)
        std::cout << " ✓\n";
    else
        std::cout << " ✗ (" << (tests_run - tests_ok) << " failed)\n";

    return (tests_ok == tests_run) ? 0 : 1;
}
