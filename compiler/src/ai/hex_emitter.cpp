#include "hex_emitter.hpp"
#include "../frontend/ast/decl.hpp"
#include "../frontend/ast/stmt.hpp"
#include "../frontend/ast/expr.hpp"

namespace sysp::ai {

std::vector<uint8_t> HexEmitter::emitir_programa(const ast::Program& prog) {
    seq_.clear();
    for (auto& m : prog.modules) emitir_decl(m.get());
    for (auto& d : prog.declarations) emitir_decl(d.get());
    return seq_;
}

void HexEmitter::emitir_tipo(const std::string& t) {
    if      (t == "i32") push(CAT_TIPO, 0x12);
    else if (t == "i64") push(CAT_TIPO, 0x13);
    else if (t == "f32") push(CAT_TIPO, 0x18);
    else if (t == "f64") push(CAT_TIPO, 0x19);
    else if (t == "bool") push(CAT_TIPO, 0x1A);
    else if (t == "string") push(CAT_TIPO, 0x1B);
    else if (!t.empty()) { push(CAT_TIPO, 0x20); push_hash(t); }
}

void HexEmitter::emitir_decl(const ast::Decl* d) {
    if (!d) return;
    if (auto* fn = dynamic_cast<const ast::FunctionDecl*>(d)) {
        push(CAT_DECLARACAO, 0x01);
        push_hash(fn->name);
        if (!fn->return_type.empty()) emitir_tipo(fn->return_type);
        if (fn->body) emitir_stmt(fn->body.get());
    }
}

void HexEmitter::emitir_stmt(const ast::Stmt* s) {
    if (!s) return;
    if (auto* v = dynamic_cast<const ast::VarDeclStmt*>(s)) {
        push(CAT_DECLARACAO, 0x06);
        for (auto& n : v->names) push_hash(n);
        emitir_tipo(v->type);
    }
    if (auto* r = dynamic_cast<const ast::ReturnStmt*>(s)) {
        push(CAT_CONTROLE, 0x07);
    }
}

void HexEmitter::emitir_expr(const ast::Expr* e) { (void)e; }

void HexEmitter::push(uint8_t a, uint8_t b) {
    seq_.push_back(a);
    seq_.push_back(b);
}

void HexEmitter::push_hash(const std::string& nome) {
    uint16_t h = 0;
    for (char c : nome) h = (uint16_t)(h * 31 + (uint8_t)c);
    seq_.push_back((uint8_t)(h >> 8));
    seq_.push_back((uint8_t)(h & 0xFF));
}

} // namespace sysp::ai
