#include "hex_emitter.hpp"
#include "../frontend/ast/decl.hpp"
#include "../frontend/ast/stmt.hpp"
#include "../frontend/ast/expr.hpp"

namespace sysp::ai {

// ── Programa ─────────────────────────────────────────────────────
std::vector<uint8_t> HexEmitter::emitir_programa(const ast::Program& prog) {
    seq_.clear();
    for (auto& m : prog.modules)      emitir_decl(m.get());
    for (auto& d : prog.declarations) emitir_decl(d.get());
    return seq_;
}

// ── Tipos ─────────────────────────────────────────────────────────
void HexEmitter::emitir_tipo(const std::string& t) {
    if      (t == "i8")     push(CAT_TIPO, 0x10);
    else if (t == "i16")    push(CAT_TIPO, 0x11);
    else if (t == "i32")    push(CAT_TIPO, 0x12);
    else if (t == "i64")    push(CAT_TIPO, 0x13);
    else if (t == "u8")     push(CAT_TIPO, 0x14);
    else if (t == "u16")    push(CAT_TIPO, 0x15);
    else if (t == "u32")    push(CAT_TIPO, 0x16);
    else if (t == "u64")    push(CAT_TIPO, 0x17);
    else if (t == "f32")    push(CAT_TIPO, 0x18);
    else if (t == "f64")    push(CAT_TIPO, 0x19);
    else if (t == "bool")   push(CAT_TIPO, 0x1A);
    else if (t == "string") push(CAT_TIPO, 0x1B);
    else if (!t.empty())  { push(CAT_TIPO, 0x20); push_hash(t); }
}

// ── Declarações ───────────────────────────────────────────────────
void HexEmitter::emitir_decl(const ast::Decl* d) {
    if (!d) return;

    if (auto* fn = dynamic_cast<const ast::FunctionDecl*>(d)) {
        push(CAT_DECLARACAO, 0x01);
        push_hash(fn->name);
        for (auto& p : fn->parameters) {
            push(CAT_DECLARACAO, 0x0A);
            push_hash(p.name);
            emitir_tipo(p.type);
        }
        if (!fn->return_type.empty()) emitir_tipo(fn->return_type);
        if (fn->body) emitir_stmt(fn->body.get());
        return;
    }

    if (auto* s = dynamic_cast<const ast::StructDecl*>(d)) {
        push(CAT_DECLARACAO, 0x02);
        push_hash(s->name);
        for (auto& f : s->fields) {
            push(CAT_DECLARACAO, 0x0B);
            push_hash(f.name);
            emitir_tipo(f.type);
        }
        return;
    }

    if (auto* e = dynamic_cast<const ast::EnumDecl*>(d)) {
        push(CAT_DECLARACAO, 0x03);
        push_hash(e->name);
        for (auto& v : e->variants) {
            push(CAT_DECLARACAO, 0x0C);
            push_hash(v.name);
        }
        return;
    }

    if (auto* t = dynamic_cast<const ast::TraitDecl*>(d)) {
        push(CAT_DECLARACAO, 0x04);
        push_hash(t->name);
        return;
    }

    if (auto* c = dynamic_cast<const ast::ConstDecl*>(d)) {
        push(CAT_DECLARACAO, 0x05);
        push_hash(c->name);
        emitir_tipo(c->type);
        if (c->value) emitir_expr(c->value.get());
        return;
    }
}

// ── Statements ────────────────────────────────────────────────────
void HexEmitter::emitir_stmt(const ast::Stmt* s) {
    if (!s) return;

    if (auto* b = dynamic_cast<const ast::BlockStmt*>(s)) {
        push(CAT_STATEMENT, 0x01);
        for (auto& st : b->statements) emitir_stmt(st.get());
        push(CAT_STATEMENT, 0x02); // end-block
        return;
    }
    if (auto* v = dynamic_cast<const ast::VarDeclStmt*>(s)) {
        push(CAT_DECLARACAO, 0x06);
        for (auto& n : v->names) push_hash(n);
        emitir_tipo(v->type);
        if (v->initializer) emitir_expr(v->initializer.get());
        return;
    }
    if (auto* a = dynamic_cast<const ast::AssignStmt*>(s)) {
        push(CAT_STATEMENT, 0x03);
        if (a->target) emitir_expr(a->target.get());
        if (a->value)  emitir_expr(a->value.get());
        return;
    }
    if (auto* r = dynamic_cast<const ast::ReturnStmt*>(s)) {
        push(CAT_CONTROLE, 0x07);
        if (r->value) emitir_expr(r->value.get());
        return;
    }
    if (auto* i = dynamic_cast<const ast::IfStmt*>(s)) {
        push(CAT_CONTROLE, 0x01);
        if (i->condition)  emitir_expr(i->condition.get());
        if (i->then_block) emitir_stmt(i->then_block.get());
        if (i->else_stmt)  emitir_stmt(i->else_stmt.get());
        return;
    }
    if (auto* w = dynamic_cast<const ast::WhileStmt*>(s)) {
        push(CAT_CONTROLE, 0x03);
        if (w->condition) emitir_expr(w->condition.get());
        if (w->body)      emitir_stmt(w->body.get());
        return;
    }
    if (auto* f = dynamic_cast<const ast::ForStmt*>(s)) {
        push(CAT_CONTROLE, 0x04);
        push_hash(f->iterator);
        if (f->iterable) emitir_expr(f->iterable.get());
        if (f->body)     emitir_stmt(f->body.get());
        return;
    }
    if (auto* e = dynamic_cast<const ast::ExpressionStmt*>(s)) {
        push(CAT_STATEMENT, 0x04);
        if (e->expression) emitir_expr(e->expression.get());
        return;
    }
    if (dynamic_cast<const ast::BreakStmt*>(s)) {
        push(CAT_CONTROLE, 0x05);
        return;
    }
    if (dynamic_cast<const ast::ContinueStmt*>(s)) {
        push(CAT_CONTROLE, 0x06);
        return;
    }
    if (auto* rg = dynamic_cast<const ast::RegionStmt*>(s)) {
        push(CAT_MEMORIA, 0x01);
        push_hash(rg->name);
        if (rg->body) emitir_stmt(rg->body.get());
        push(CAT_MEMORIA, 0x02); // end-region
        return;
    }
    if (auto* u = dynamic_cast<const ast::UnsafeStmt*>(s)) {
        push(CAT_MEMORIA, 0x03);
        if (u->body) emitir_stmt(u->body.get());
        push(CAT_MEMORIA, 0x04); // end-unsafe
        return;
    }
}

// ── Expressions ───────────────────────────────────────────────────
void HexEmitter::emitir_expr(const ast::Expr* e) {
    if (!e) return;

    if (auto* lit = dynamic_cast<const ast::LiteralExpr*>(e)) {
        switch (lit->kind) {
            case ast::LiteralKind::Int:    push(CAT_LITERAL, 0x01); break;
            case ast::LiteralKind::Float:  push(CAT_LITERAL, 0x02); break;
            case ast::LiteralKind::String: push(CAT_LITERAL, 0x03); break;
            case ast::LiteralKind::Bool:   push(CAT_LITERAL, 0x04); break;
        }
        push_hash(lit->value);
        return;
    }

    if (auto* id = dynamic_cast<const ast::IdentifierExpr*>(e)) {
        push(CAT_EXPRESSAO, 0x01);
        push_hash(id->name);
        return;
    }

    if (auto* bin = dynamic_cast<const ast::BinaryExpr*>(e)) {
        push(CAT_OPERADOR, op_byte(bin->op));
        emitir_expr(bin->left.get());
        emitir_expr(bin->right.get());
        return;
    }

    if (auto* un = dynamic_cast<const ast::UnaryExpr*>(e)) {
        push(CAT_OPERADOR, 0xF0);
        push_hash(un->op);
        emitir_expr(un->operand.get());
        return;
    }

    if (auto* call = dynamic_cast<const ast::CallExpr*>(e)) {
        push(CAT_EXPRESSAO, 0x02);
        emitir_expr(call->callee.get());
        for (auto& arg : call->arguments) emitir_expr(arg.get());
        push(CAT_EXPRESSAO, 0x0F); // end-args
        return;
    }

    if (auto* mem = dynamic_cast<const ast::MemberExpr*>(e)) {
        push(CAT_EXPRESSAO, 0x03);
        emitir_expr(mem->object.get());
        push_hash(mem->field);
        return;
    }

    if (auto* idx = dynamic_cast<const ast::IndexExpr*>(e)) {
        push(CAT_EXPRESSAO, 0x04);
        emitir_expr(idx->object.get());
        emitir_expr(idx->index.get());
        return;
    }

    if (auto* rng = dynamic_cast<const ast::RangeExpr*>(e)) {
        push(CAT_EXPRESSAO, 0x05);
        emitir_expr(rng->start.get());
        emitir_expr(rng->end.get());
        push(CAT_EXPRESSAO, rng->inclusive ? 0x06 : 0x07);
        return;
    }

    if (auto* cast = dynamic_cast<const ast::CastExpr*>(e)) {
        push(CAT_EXPRESSAO, 0x08);
        emitir_expr(cast->expr.get());
        emitir_tipo(cast->target_type);
        return;
    }

    // Fallback: unknown expr node
    push(CAT_EXPRESSAO, 0xFF);
}

// ── Helpers ───────────────────────────────────────────────────────
void HexEmitter::push(uint8_t a, uint8_t b) {
    seq_.push_back(a);
    seq_.push_back(b);
}

void HexEmitter::push_hash(const std::string& nome) {
    uint16_t h = 0;
    for (char c : nome) h = static_cast<uint16_t>(h * 31 + static_cast<uint8_t>(c));
    seq_.push_back(static_cast<uint8_t>(h >> 8));
    seq_.push_back(static_cast<uint8_t>(h & 0xFF));
}

uint8_t HexEmitter::op_byte(const std::string& op) {
    if (op == "+")  return 0x10;
    if (op == "-")  return 0x11;
    if (op == "*")  return 0x12;
    if (op == "/")  return 0x13;
    if (op == "%")  return 0x14;
    if (op == "==") return 0x20;
    if (op == "!=") return 0x21;
    if (op == "<")  return 0x22;
    if (op == ">")  return 0x23;
    if (op == "<=") return 0x24;
    if (op == ">=") return 0x25;
    if (op == "&&") return 0x30;
    if (op == "||") return 0x31;
    if (op == "&")  return 0x40;
    if (op == "|")  return 0x41;
    if (op == "^")  return 0x42;
    if (op == "<<") return 0x43;
    if (op == ">>") return 0x44;
    return 0xEE; // unknown op
}

} // namespace sysp::ai
