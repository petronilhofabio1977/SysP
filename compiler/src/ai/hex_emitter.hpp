#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include "../frontend/ast/decl.hpp"
#include "../frontend/ast/stmt.hpp"
#include "../frontend/ast/expr.hpp"

namespace sysp::ai {

// Categorias semânticas
constexpr uint8_t CAT_DECLARACAO = 0x01;
constexpr uint8_t CAT_TIPO       = 0x02;
constexpr uint8_t CAT_EXPRESSAO  = 0x03;
constexpr uint8_t CAT_STATEMENT  = 0x04;
constexpr uint8_t CAT_OPERADOR   = 0x05;
constexpr uint8_t CAT_LITERAL    = 0x06;
constexpr uint8_t CAT_CONTROLE   = 0x07;
constexpr uint8_t CAT_MEMORIA    = 0x08;
constexpr uint8_t CAT_ERRO       = 0x09;
constexpr uint8_t CAT_RELACAO    = 0x0A;

class HexEmitter {
public:
    std::vector<uint8_t> emitir_programa(const ast::Program& prog);

private:
    std::vector<uint8_t> seq_;
    void emitir_decl(const ast::Decl* d);
    void emitir_stmt(const ast::Stmt* s);
    void emitir_expr(const ast::Expr* e);
    void emitir_tipo(const std::string& t);
    void push(uint8_t a, uint8_t b);
    void push_hash(const std::string& nome);
    uint8_t op_byte(const std::string& op);
};

} // namespace sysp::ai
