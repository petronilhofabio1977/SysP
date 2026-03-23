#include "ast_to_cfg.hpp"
#include <iostream>

namespace sysp::cfg {

using namespace sysp::ir;
using namespace sysp::ast;

BasicBlock* ASTToCFG::new_block(const std::string& label) {
    auto b = std::make_unique<BasicBlock>(next_id_++, label.empty() ? "B" + std::to_string(next_id_-1) : label);
    auto* ptr = b.get();
    blocks_.push_back(std::move(b));
    return ptr;
}

void ASTToCFG::set_current(BasicBlock* b) { current_ = b; }

void ASTToCFG::connect(BasicBlock* from, BasicBlock* to) {
    from->successors.push_back(to);
    to->predecessors.push_back(from);
}

std::string ASTToCFG::new_label(const std::string& prefix) {
    return prefix + std::to_string(label_counter_++);
}

std::string ASTToCFG::new_temp() {
    return "%t" + std::to_string(temp_counter_++);
}

std::vector<std::unique_ptr<BasicBlock>>
ASTToCFG::build(const FunctionDecl* fn) {
    blocks_.clear();
    next_id_ = 0;
    label_counter_ = 0;
    temp_counter_  = 0;

    auto* entry = new_block("entry");
    set_current(entry);

    // Register parameters as initialized
    for (auto& p : fn->parameters) {
        if (!p.is_self) {
            auto* i = current_->add(Opcode::Alloc);
            i->var_name  = p.name;
            i->type_name = p.type;
            auto* s = current_->add(Opcode::Store);
            s->var_name  = p.name;
            current_->def[p.name] = true;
        }
    }

    if (fn->body) emit_block(fn->body.get());

    // Add implicit return if last block has no terminator
    if (current_ && !current_->terminator())
        current_->add(Opcode::Return);

    return std::move(blocks_);
}

std::vector<std::unique_ptr<BasicBlock>>
ASTToCFG::build_from_block(const BlockStmt* block) {
    blocks_.clear();
    next_id_ = 0;
    auto* entry = new_block("entry");
    set_current(entry);
    if (block) emit_block(block);
    if (current_ && !current_->terminator())
        current_->add(Opcode::Return);
    return std::move(blocks_);
}

void ASTToCFG::emit_block(const BlockStmt* block) {
    if (!block) return;
    for (auto& stmt : block->statements)
        emit_stmt(stmt.get());
}

void ASTToCFG::emit_stmt(const Stmt* stmt) {
    if (!stmt) return;

    if (auto* s = dynamic_cast<const VarDeclStmt*>(stmt))
        { emit_var_decl(s); return; }
    if (auto* s = dynamic_cast<const AssignStmt*>(stmt))
        { emit_assign(s); return; }
    if (auto* s = dynamic_cast<const IfStmt*>(stmt))
        { emit_if(s); return; }
    if (auto* s = dynamic_cast<const WhileStmt*>(stmt))
        { emit_while(s); return; }
    if (auto* s = dynamic_cast<const ForStmt*>(stmt))
        { emit_for(s); return; }
    if (auto* s = dynamic_cast<const LoopStmt*>(stmt))
        { emit_loop(s); return; }
    if (auto* s = dynamic_cast<const ReturnStmt*>(stmt))
        { emit_return(s); return; }
    if (auto* s = dynamic_cast<const RegionStmt*>(stmt))
        { emit_region(s); return; }
    if (auto* s = dynamic_cast<const UnsafeStmt*>(stmt))
        { emit_unsafe(s); return; }
    if (auto* s = dynamic_cast<const ExpressionStmt*>(stmt))
        { emit_expr_stmt(s); return; }
    if (auto* s = dynamic_cast<const BlockStmt*>(stmt))
        { emit_block(s); return; }

    if (dynamic_cast<const BreakStmt*>(stmt)) {
        if (break_target_) {
            auto* i = current_->add(Opcode::Jump);
            i->label = break_target_->label;
            connect(current_, break_target_);
        }
        return;
    }

    if (dynamic_cast<const ContinueStmt*>(stmt)) {
        if (continue_target_) {
            auto* i = current_->add(Opcode::Jump);
            i->label = continue_target_->label;
            connect(current_, continue_target_);
        }
        return;
    }
}

void ASTToCFG::emit_var_decl(const VarDeclStmt* stmt) {
    for (auto& name : stmt->names) {
        // Declare the variable
        auto* alloc = current_->add(Opcode::Alloc);
        alloc->var_name  = name;
        alloc->type_name = stmt->type;

        if (stmt->initializer) {
            // Has initializer — store it (variable is initialized)
            emit_expr(stmt->initializer.get(), name);
            auto* store = current_->add(Opcode::Store);
            store->var_name = name;
            current_->def[name] = true;
        } else {
            // No initializer — variable is declared but NOT initialized
            // This is the key for uninitialized-use detection
            current_->def[name] = false; // declared but uninitialized
        }
    }
}

void ASTToCFG::emit_assign(const AssignStmt* stmt) {
    emit_expr(stmt->value.get());
    if (auto* id = dynamic_cast<const IdentifierExpr*>(stmt->target.get())) {
        auto* store = current_->add(Opcode::Store);
        store->var_name = id->name;
        current_->def[id->name] = true;
    }
}

void ASTToCFG::emit_if(const IfStmt* stmt) {
    // Emit condition
    std::string cond = new_temp();
    emit_expr(stmt->condition.get(), cond);

    // Create blocks
    auto* then_block = new_block(new_label("if_then_"));
    auto* else_block = stmt->else_stmt ? new_block(new_label("if_else_")) : nullptr;
    auto* merge_block = new_block(new_label("if_merge_"));

    // Branch instruction
    auto* branch = current_->add(Opcode::Branch);
    branch->true_block  = then_block;
    branch->false_block = else_block ? else_block : merge_block;
    connect(current_, then_block);
    connect(current_, else_block ? else_block : merge_block);

    // Then block
    set_current(then_block);
    emit_block(stmt->then_block.get());
    if (!current_->terminator()) {
        current_->add(Opcode::Jump)->label = merge_block->label;
        connect(current_, merge_block);
    }

    // Else block
    if (else_block) {
        set_current(else_block);
        emit_stmt(stmt->else_stmt.get());
        if (!current_->terminator()) {
            current_->add(Opcode::Jump)->label = merge_block->label;
            connect(current_, merge_block);
        }
    }

    set_current(merge_block);
}

void ASTToCFG::emit_while(const WhileStmt* stmt) {
    auto* header = new_block(new_label("while_header_"));
    auto* body   = new_block(new_label("while_body_"));
    auto* exit   = new_block(new_label("while_exit_"));

    current_->add(Opcode::Jump)->label = header->label;
    connect(current_, header);

    set_current(header);
    std::string cond = new_temp();
    emit_expr(stmt->condition.get(), cond);
    auto* br = current_->add(Opcode::Branch);
    br->true_block  = body;
    br->false_block = exit;
    connect(header, body);
    connect(header, exit);

    auto* old_break    = break_target_;
    auto* old_continue = continue_target_;
    break_target_    = exit;
    continue_target_ = header;

    set_current(body);
    emit_block(stmt->body.get());
    if (!current_->terminator()) {
        current_->add(Opcode::Jump)->label = header->label;
        connect(current_, header);
    }

    break_target_    = old_break;
    continue_target_ = old_continue;

    set_current(exit);
}

void ASTToCFG::emit_for(const ForStmt* stmt) {
    auto* header = new_block(new_label("for_header_"));
    auto* body   = new_block(new_label("for_body_"));
    auto* exit   = new_block(new_label("for_exit_"));

    current_->add(Opcode::Jump)->label = header->label;
    connect(current_, header);

    set_current(header);
    auto* br = current_->add(Opcode::Branch);
    br->true_block  = body;
    br->false_block = exit;
    connect(header, body);
    connect(header, exit);

    // Iterator variable declared in loop body
    auto* alloc = body->add(Opcode::Alloc);
    alloc->var_name = stmt->iterator;
    auto* store = body->add(Opcode::Store);
    store->var_name = stmt->iterator;
    body->def[stmt->iterator] = true;

    auto* old_break    = break_target_;
    auto* old_continue = continue_target_;
    break_target_    = exit;
    continue_target_ = header;

    set_current(body);
    emit_block(stmt->body.get());
    if (!current_->terminator()) {
        current_->add(Opcode::Jump)->label = header->label;
        connect(current_, header);
    }

    break_target_    = old_break;
    continue_target_ = old_continue;

    set_current(exit);
}

void ASTToCFG::emit_loop(const LoopStmt* stmt) {
    auto* body = new_block(new_label("loop_body_"));
    auto* exit = new_block(new_label("loop_exit_"));

    current_->add(Opcode::Jump)->label = body->label;
    connect(current_, body);

    auto* old_break    = break_target_;
    auto* old_continue = continue_target_;
    break_target_    = exit;
    continue_target_ = body;

    set_current(body);
    emit_block(stmt->body.get());
    if (!current_->terminator()) {
        current_->add(Opcode::Jump)->label = body->label;
        connect(current_, body);
    }

    break_target_    = old_break;
    continue_target_ = old_continue;

    set_current(exit);
}

void ASTToCFG::emit_return(const ReturnStmt* stmt) {
    if (stmt->value) emit_expr(stmt->value.get());
    current_->add(Opcode::Return);
}

void ASTToCFG::emit_region(const RegionStmt* stmt) {
    current_->add(Opcode::RegionEnter)->label = stmt->name;
    emit_block(stmt->body.get());
    current_->add(Opcode::RegionExit)->label = stmt->name;
}

void ASTToCFG::emit_unsafe(const UnsafeStmt* stmt) {
    emit_block(stmt->body.get());
}

void ASTToCFG::emit_expr_stmt(const ExpressionStmt* stmt) {
    emit_expr(stmt->expression.get());
}

void ASTToCFG::emit_expr(const Expr* expr, const std::string& result) {
    if (!expr) return;

    if (auto* e = dynamic_cast<const LiteralExpr*>(expr)) {
        switch (e->kind) {
        case LiteralKind::Int: {
            auto* i = current_->add(Opcode::ConstInt);
            try { i->const_int = std::stoll(e->value); } catch (...) {}
            i->result_name = result;
            break;
        }
        case LiteralKind::Float: {
            auto* i = current_->add(Opcode::ConstFloat);
            try { i->const_float = std::stod(e->value); } catch (...) {}
            i->result_name = result;
            break;
        }
        case LiteralKind::String: {
            auto* i = current_->add(Opcode::ConstStr);
            i->const_str   = e->value;
            i->result_name = result;
            break;
        }
        case LiteralKind::Bool: {
            auto* i = current_->add(Opcode::ConstBool);
            i->const_bool  = (e->value == "true");
            i->result_name = result;
            break;
        }
        }
        return;
    }

    if (auto* e = dynamic_cast<const IdentifierExpr*>(expr)) {
        auto* load = current_->add_load(e->name);
        load->result_name = result.empty() ? new_temp() : result;
        return;
    }

    if (auto* e = dynamic_cast<const BinaryExpr*>(expr)) {
        std::string lt = new_temp(), rt = new_temp();
        emit_expr(e->left.get(),  lt);
        emit_expr(e->right.get(), rt);
        Opcode op = Opcode::Add;
        if      (e->op == "+")  op = Opcode::Add;
        else if (e->op == "-")  op = Opcode::Sub;
        else if (e->op == "*")  op = Opcode::Mul;
        else if (e->op == "/")  op = Opcode::Div;
        else if (e->op == "%")  op = Opcode::Mod;
        else if (e->op == "==") op = Opcode::CmpEQ;
        else if (e->op == "!=") op = Opcode::CmpNE;
        else if (e->op == "<")  op = Opcode::CmpLT;
        else if (e->op == "<=") op = Opcode::CmpLE;
        else if (e->op == ">")  op = Opcode::CmpGT;
        else if (e->op == ">=") op = Opcode::CmpGE;
        else if (e->op == "&&") op = Opcode::And;
        else if (e->op == "||") op = Opcode::Or;
        auto* instr = current_->add(op);
        instr->result_name = result.empty() ? new_temp() : result;
        return;
    }

    if (auto* e = dynamic_cast<const CallExpr*>(expr)) {
        for (auto& arg : e->arguments) emit_expr(arg.get());
        auto* call = current_->add(Opcode::Call);
        call->result_name = result.empty() ? new_temp() : result;
        return;
    }

    if (auto* e = dynamic_cast<const MoveExpr*>(expr)) {
        emit_expr(e->expr.get());
        auto* mv = current_->add(Opcode::Move);
        mv->result_name = result;
        return;
    }

    // Default: emit as nop
    auto* nop = current_->add(Opcode::Nop);
    nop->result_name = result;
}

} // namespace sysp::cfg
