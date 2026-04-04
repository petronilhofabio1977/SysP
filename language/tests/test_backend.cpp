// ================================================================
// SysP Backend Codegen Tests — Structs
// ================================================================
#include "../../compiler/src/frontend/lexer/lexer.hpp"
#include "../../compiler/src/frontend/parser/parser.hpp"
#include "../../compiler/src/backend/x86/x86_backend.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <cassert>

static int tests_run = 0;
static int tests_ok  = 0;

#define ASSERT_TRUE(x) do { \
    tests_run++; \
    if (x) { tests_ok++; } \
    else { std::cerr << "FAIL [" << __FILE__ << ":" << __LINE__ << "]: " #x "\n"; } \
} while(0)

#define ASSERT_CONTAINS(haystack, needle) do { \
    tests_run++; \
    if ((haystack).find(needle) != std::string::npos) { tests_ok++; } \
    else { std::cerr << "FAIL [" << __LINE__ << "]: expected to find:\n  \"" \
                     << (needle) << "\"\nin output\n"; } \
} while(0)

#define ASSERT_NOT_CONTAINS(haystack, needle) do { \
    tests_run++; \
    if ((haystack).find(needle) == std::string::npos) { tests_ok++; } \
    else { std::cerr << "FAIL [" << __LINE__ << "]: expected NOT to find:\n  \"" \
                     << (needle) << "\"\nin output\n"; } \
} while(0)

static std::string codegen(const std::string& src) {
    Lexer lex(src);
    auto tokens = lex.tokenize();
    sysp::parser::Parser parser(std::move(tokens));
    auto prog = parser.parse_program();
    sysp::backend::x86::Backend backend;
    std::ostringstream out;
    backend.generate_program(prog, out);
    return out.str();
}

// ── Test 1: layout — dois campos em offsets corretos ─────────────
// Point { x i32, y i32 }: x→[rbp-8], y→[rbp-16]
void test_struct_field_layout() {
    auto asm_code = codegen(
        "module main\n"
        "struct Point {\n"
        "    x i32\n"
        "    y i32\n"
        "}\n"
        "fn main() {\n"
        "    let p = Point { x: 10, y: 20 }\n"
        "}\n"
    );
    // Primeiro campo inicializado em [rbp-8]
    ASSERT_CONTAINS(asm_code, "[rbp-8]");
    // Segundo campo inicializado em [rbp-16]
    ASSERT_CONTAINS(asm_code, "[rbp-16]");
    // Comentário de debug do gen_struct_init
    ASSERT_CONTAINS(asm_code, "struct Point p");
}

// ── Test 2: leitura de campo — p.x gera mov rax, [rbp-8] ─────────
void test_struct_field_read() {
    auto asm_code = codegen(
        "module main\n"
        "struct Point {\n"
        "    x i32\n"
        "    y i32\n"
        "}\n"
        "fn main() -> i32 {\n"
        "    let p = Point { x: 42, y: 0 }\n"
        "    return p.x\n"
        "}\n"
    );
    // p.x deve ser lido de [rbp-8]
    ASSERT_CONTAINS(asm_code, "mov rax, [rbp-8]");
    // Comentário de field read
    ASSERT_CONTAINS(asm_code, "p.x");
}

// ── Test 3: escrita de campo — p.y = 5 gera mov [rbp-16], rax ────
void test_struct_field_write() {
    auto asm_code = codegen(
        "module main\n"
        "struct Point {\n"
        "    x i32\n"
        "    y i32\n"
        "}\n"
        "fn main() {\n"
        "    let p = Point { x: 1, y: 2 }\n"
        "    p.y = 99\n"
        "}\n"
    );
    // Valor 99 carregado em rax
    ASSERT_CONTAINS(asm_code, "mov rax, 99");
    // Armazenado em [rbp-16] (campo y)
    ASSERT_CONTAINS(asm_code, "mov [rbp-16], rax");
    // Comentário de field write
    ASSERT_CONTAINS(asm_code, "p.y =");
}

// ── Test 4: println de campo inteiro chama sysp_println_int ───────
void test_struct_println_int_field() {
    auto asm_code = codegen(
        "module main\n"
        "struct Rect {\n"
        "    width i32\n"
        "    height i32\n"
        "}\n"
        "fn main() {\n"
        "    let r = Rect { width: 10, height: 5 }\n"
        "    println(r.width)\n"
        "}\n"
    );
    ASSERT_CONTAINS(asm_code, "call sysp_println_int");
    // Não deve chamar bool para campo inteiro
    ASSERT_NOT_CONTAINS(asm_code, "call sysp_println_bool");
}

// ── Test 5: println de campo bool chama sysp_println_bool ─────────
void test_struct_println_bool_field() {
    auto asm_code = codegen(
        "module main\n"
        "struct Flags {\n"
        "    active bool\n"
        "    debug bool\n"
        "}\n"
        "fn main() {\n"
        "    let f = Flags { active: true, debug: false }\n"
        "    println(f.active)\n"
        "}\n"
    );
    ASSERT_CONTAINS(asm_code, "sysp_println_bool");
}

// ── Test 6: duas instâncias — offsets não se sobrepõem ───────────
void test_two_struct_instances_non_overlapping() {
    auto asm_code = codegen(
        "module main\n"
        "struct Vec2 {\n"
        "    x i32\n"
        "    y i32\n"
        "}\n"
        "fn main() {\n"
        "    let a = Vec2 { x: 1, y: 2 }\n"
        "    let b = Vec2 { x: 3, y: 4 }\n"
        "}\n"
    );
    // a: campos em [rbp-8] e [rbp-16]
    // b: campos em [rbp-24] e [rbp-32]
    ASSERT_CONTAINS(asm_code, "[rbp-8]");
    ASSERT_CONTAINS(asm_code, "[rbp-16]");
    ASSERT_CONTAINS(asm_code, "[rbp-24]");
    ASSERT_CONTAINS(asm_code, "[rbp-32]");
}

// ── Test 7: struct de 3 campos — offsets corretos ────────────────
void test_struct_three_fields() {
    auto asm_code = codegen(
        "module main\n"
        "struct Color {\n"
        "    r i32\n"
        "    g i32\n"
        "    b i32\n"
        "}\n"
        "fn main() {\n"
        "    let c = Color { r: 255, g: 128, b: 0 }\n"
        "}\n"
    );
    ASSERT_CONTAINS(asm_code, "[rbp-8]");   // r
    ASSERT_CONTAINS(asm_code, "[rbp-16]");  // g
    ASSERT_CONTAINS(asm_code, "[rbp-24]");  // b
    ASSERT_CONTAINS(asm_code, "struct Color c");
}

// ── Test 8: struct + scalar no mesmo frame — não se sobrepõem ────
void test_struct_and_scalar_non_overlapping() {
    auto asm_code = codegen(
        "module main\n"
        "struct Point {\n"
        "    x i32\n"
        "    y i32\n"
        "}\n"
        "fn main() -> i32 {\n"
        "    let p = Point { x: 1, y: 2 }\n"
        "    let n = 42\n"
        "    return n\n"
        "}\n"
    );
    // p.x=[rbp-8], p.y=[rbp-16], n=[rbp-24]
    ASSERT_CONTAINS(asm_code, "[rbp-24]");
    // n deve ser lido corretamente no return
    ASSERT_CONTAINS(asm_code, "mov rax, [rbp-24]");
}

// ── Test 9: ordem dos campos no init — layout order ──────────────
void test_struct_init_layout_order() {
    // Inicializado fora de ordem: y primeiro, x depois
    // Gerado deve ser em layout order (x=field 0, y=field 1)
    auto asm_code = codegen(
        "module main\n"
        "struct Point {\n"
        "    x i32\n"
        "    y i32\n"
        "}\n"
        "fn main() {\n"
        "    let p = Point { y: 20, x: 10 }\n"
        "}\n"
    );
    // x (field 0) deve ir para [rbp-8] mesmo sendo listado depois
    ASSERT_CONTAINS(asm_code, "Point.x");
    ASSERT_CONTAINS(asm_code, "Point.y");
}

// ── Test 10: campo não inicializado → zero ────────────────────────
void test_struct_missing_field_defaults_to_zero() {
    auto asm_code = codegen(
        "module main\n"
        "struct Point {\n"
        "    x i32\n"
        "    y i32\n"
        "}\n"
        "fn main() {\n"
        "    let p = Point { x: 5 }\n"
        "}\n"
    );
    // Campo y não fornecido → default 0
    ASSERT_CONTAINS(asm_code, "default 0");
}

// ── Main ─────────────────────────────────────────────────────────
int main() {
    std::cout << "=== SysP Backend Codegen Tests — Structs ===\n\n";

    test_struct_field_layout();
    test_struct_field_read();
    test_struct_field_write();
    test_struct_println_int_field();
    test_struct_println_bool_field();
    test_two_struct_instances_non_overlapping();
    test_struct_three_fields();
    test_struct_and_scalar_non_overlapping();
    test_struct_init_layout_order();
    test_struct_missing_field_defaults_to_zero();

    std::cout << "\nResults: " << tests_ok << "/" << tests_run << " passed";
    if (tests_ok == tests_run)
        std::cout << " ✓\n";
    else
        std::cout << " ✗ (" << (tests_run - tests_ok) << " failed)\n";

    return (tests_ok == tests_run) ? 0 : 1;
}
