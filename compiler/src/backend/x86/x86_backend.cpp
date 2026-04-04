#include "x86_backend.hpp"
#include "../../frontend/ast/stmt.hpp"
#include "../../frontend/ast/expr.hpp"
#include <iostream>
#include <sstream>

namespace sysp::backend::x86 {

    using namespace sysp::ast;

    // ================================================================
    // Constructor
    // ================================================================

    Backend::Backend() {}

    // ================================================================
    // Old interface — kept for compatibility
    // ================================================================

    void Backend::generate(const sysp::metatron::Graph& graph, std::ostream& out) {
        (void)graph;
        out << "; SysP compiler output (legacy)\n";
        out << "section .text\n";
        out << "global _start\n";
        out << "_start:\n";
        out << "    mov rax, 60\n";
        out << "    xor rdi, rdi\n";
        out << "    syscall\n";
    }

    // ================================================================
    // Helpers
    // ================================================================

    std::string Backend::new_string_label(const std::string& value) {
        std::string label = "str_" + std::to_string(str_counter_++);
        string_constants_.push_back({ label, value });
        return label;
    }

    std::string Backend::new_label(const std::string& prefix) {
        return prefix + "_" + std::to_string(label_counter_++);
    }

    int Backend::alloc_stack_var(const std::string& name, int bytes) {
        stack_offset_ += bytes;
        var_offsets_[name] = stack_offset_;
        return stack_offset_;
    }

    void Backend::emit_function_prologue(const std::string& name, int stack_size, std::ostream& out) {
        out << "\n" << name << ":\n";
        out << "    push rbp\n";
        out << "    mov rbp, rsp\n";
        if (stack_size > 0)
            out << "    sub rsp, " << stack_size << "\n";
    }

    void Backend::emit_function_epilogue(std::ostream& out) {
        out << "    mov rsp, rbp\n";
        out << "    pop rbp\n";
        out << "    ret\n";
    }

    // ================================================================
    // Data section
    // ================================================================

    void Backend::emit_data_section(std::ostream& out) {
        if (string_constants_.empty()) return;

        out << "\nsection .data\n";
        for (auto& sc : string_constants_) {
            out << "    " << sc.label << " db ";
            // Emit each character
            bool first = true;
            for (unsigned char c : sc.value) {
                if (!first) out << ", ";
                out << (int)c;
                first = false;
            }
            out << ", 10\n"; // newline
            out << "    " << sc.label << "_len equ $ - " << sc.label << "\n";
        }
    }

    // ================================================================
    // Built-in helper functions emitted at the end
    // ================================================================

    // println_int: prints integer in rdi followed by newline
    void Backend::emit_println_int(std::ostream& out) {
        out << R"(
; println_int(rdi = value)
sysp_println_int:
    push rbp
    mov rbp, rsp
    sub rsp, 32

    ; handle zero
    cmp rdi, 0
    jne .pi_nonzero
    mov byte [rbp-1], 10    ; newline
    mov byte [rbp-2], '0'
    lea rsi, [rbp-2]
    mov rdx, 2
    mov rax, 1
    mov rdi, 1
    syscall
    jmp .pi_done

.pi_nonzero:
    ; handle negative
    xor r8, r8
    cmp rdi, 0
    jge .pi_positive
    inc r8
    neg rdi

    .pi_positive:
    ; convert to string in buffer (backwards)
    lea rcx, [rbp-1]
    mov byte [rcx], 10      ; newline first
    dec rcx
    mov rax, rdi

    .pi_loop:
    xor rdx, rdx
    mov rbx, 10
    div rbx
    add dl, '0'
    mov [rcx], dl
    dec rcx
    test rax, rax
    jnz .pi_loop

    ; add minus if negative
    test r8, r8
    jz .pi_emit
    mov byte [rcx], '-'
    dec rcx

    .pi_emit:
    inc rcx
    lea rdx, [rbp-1]
    inc rdx
    sub rdx, rcx
    mov rsi, rcx
    mov rax, 1
    mov rdi, 1
    syscall

    .pi_done:
    mov rsp, rbp
    pop rbp
    ret
    )";
    }

    // emit_println_float: stub — floats printed as integers for now
    void Backend::emit_println_float(std::ostream& out) {
        (void)out; // TODO: implement using xmm registers
    }

    // println_bool: prints "true\n" or "false\n" based on rdi
    void Backend::emit_println_bool(std::ostream& out) {
        out << R"(
; println_bool(rdi = 0 or 1)
sysp_println_bool:
    test rdi, rdi
    jz .pb_false
    mov rax, 1
    mov rdi, 1
    mov rsi, sysp_str_true
    mov rdx, 5
    syscall
    ret
.pb_false:
    mov rax, 1
    mov rdi, 1
    mov rsi, sysp_str_false
    mov rdx, 6
    syscall
    ret
)";
    }

    // ================================================================
    // Main program generation
    // ================================================================

    // ================================================================
    // Struct layout collection
    // ================================================================

    void Backend::collect_struct_layouts(const sysp::ast::Program& program) {
        struct_layouts_.clear();
        for (auto& decl : program.declarations) {
            if (auto* s = dynamic_cast<const StructDecl*>(decl.get())) {
                StructLayout layout;
                for (auto& f : s->fields) {
                    layout.field_names.push_back(f.name);
                    layout.field_types.push_back(f.type);
                }
                struct_layouts_[s->name] = std::move(layout);
            }
        }
    }

    void Backend::generate_program(const sysp::ast::Program& program, std::ostream& out) {
        // Reset state
        string_constants_.clear();
        str_counter_   = 0;
        label_counter_ = 0;
        var_offsets_.clear();
        stack_offset_ = 0;

        // Build struct layout table before any codegen
        collect_struct_layouts(program);

        // Collect all code into a buffer first (we need string constants)
        std::ostringstream code;

        code << "section .text\n";
        code << "global _start\n";

        // Generate all function declarations
        for (auto& decl : program.declarations) {
            if (auto* fn = dynamic_cast<const FunctionDecl*>(decl.get())) {
                stack_offset_ = 0;
                var_offsets_.clear();
                gen_function(fn, code);
            }
        }

        // Entry point _start → calls main
        code << "\n_start:\n";
        code << "    call main\n";
        code << "    mov rax, 60\n";
        code << "    xor rdi, rdi\n";
        code << "    syscall\n";

        // Emit helper functions
        emit_println_int(code);
        emit_println_bool(code);

        // Now emit data section (collected string constants)
        emit_data_section(out);

        // Emit bool string literals
        out << "\n    sysp_str_true  db 't','r','u','e',10\n";
        out << "    sysp_str_false db 'f','a','l','s','e',10\n";

        // Emit the code
        out << code.str();
    }

    // ================================================================
    // Function generation
    // ================================================================

    void Backend::gen_function(const FunctionDecl* fn, std::ostream& out) {
        // Reset per-function state — prevents stack offset accumulating
        // across multiple functions compiled in the same Backend instance.
        stack_offset_ = 0;
        var_offsets_.clear();
        var_types_.clear();

        // Estimate stack size (will refine later)
        int estimated_stack = 256;

        emit_function_prologue(fn->name, estimated_stack, out);

        // Bind parameters to stack slots
        // System V ABI: rdi, rsi, rdx, rcx, r8, r9
        static const char* param_regs[] = {"rdi","rsi","rdx","rcx","r8","r9"};
        int param_idx = 0;
        for (auto& param : fn->parameters) {
            if (param.is_self) continue;
            if (param_idx < 6) {
                int off = alloc_stack_var(param.name);
                out << "    mov [rbp-" << off << "], " << param_regs[param_idx] << "\n";
                param_idx++;
            }
        }

        if (fn->body)
            gen_block(fn->body.get(), out);

        // Default return 0 if no explicit return
        out << "    xor rax, rax\n";
        emit_function_epilogue(out);
    }

    // ================================================================
    // Block generation
    // ================================================================

    void Backend::gen_block(const BlockStmt* block, std::ostream& out) {
        if (!block) return;
        for (auto& stmt : block->statements)
            gen_stmt(stmt.get(), out);
    }

    // ================================================================
    // Statement generation
    // ================================================================

    void Backend::gen_stmt(const Stmt* stmt, std::ostream& out) {
        if (!stmt) return;

        if (auto* s = dynamic_cast<const VarDeclStmt*>(stmt))
        { gen_var_decl(s, out); return; }

        if (auto* s = dynamic_cast<const AssignStmt*>(stmt))
        { gen_assign(s, out); return; }

        if (auto* s = dynamic_cast<const ReturnStmt*>(stmt))
        { gen_return(s, out); return; }

        if (auto* s = dynamic_cast<const IfStmt*>(stmt))
        { gen_if(s, out); return; }

        if (auto* s = dynamic_cast<const WhileStmt*>(stmt))
        { gen_while(s, out); return; }

        if (auto* s = dynamic_cast<const ForStmt*>(stmt))
        { gen_for(s, out); return; }

        if (auto* s = dynamic_cast<const ExpressionStmt*>(stmt))
        { gen_expr_stmt(s, out); return; }

        if (auto* s = dynamic_cast<const BlockStmt*>(stmt))
        { gen_block(s, out); return; }

        if (auto* s = dynamic_cast<const BreakStmt*>(stmt)) {
            (void)s;
            out << "    ; break\n";
            return;
        }

        if (auto* s = dynamic_cast<const ContinueStmt*>(stmt)) {
            (void)s;
            out << "    ; continue\n";
            return;
        }

        if (auto* s = dynamic_cast<const PanicStmt*>(stmt)) {
            (void)s;
            out << "    ; panic\n";
            out << "    mov rax, 60\n";
            out << "    mov rdi, 1\n";
            out << "    syscall\n";
            return;
        }

        if (auto* s = dynamic_cast<const RegionStmt*>(stmt)) {
            out << "    ; region " << s->name << " begin\n";
            gen_block(s->body.get(), out);
            out << "    ; region " << s->name << " end\n";
            return;
        }

        if (auto* s = dynamic_cast<const UnsafeStmt*>(stmt)) {
            out << "    ; unsafe begin\n";
            gen_block(s->body.get(), out);
            out << "    ; unsafe end\n";
            return;
        }

        out << "    ; unhandled statement\n";
    }

    // ================================================================
    // Struct generation
    // ================================================================

    // Allocates n_fields × 8 bytes on the stack and initializes each field.
    // var_offsets_[var_name] = offset of field 0 from rbp.
    // Field i lives at [rbp - (var_offsets_[var_name] + i*8)].
    void Backend::gen_struct_init(const std::string& var_name,
                                  const StructInitExpr* si,
                                  std::ostream& out) {
        auto it = struct_layouts_.find(si->type_name);
        int n_fields = (it != struct_layouts_.end())
                       ? static_cast<int>(it->second.field_names.size())
                       : static_cast<int>(si->fields.size());

        if (n_fields == 0) return;

        // Allocate n_fields slots (8 bytes each); field 0 at [rbp - base].
        stack_offset_ += 8;
        int base_off = stack_offset_;
        if (n_fields > 1) stack_offset_ += (n_fields - 1) * 8;

        var_offsets_[var_name] = base_off;
        var_types_[var_name]   = si->type_name;

        out << "    ; struct " << si->type_name << " " << var_name
            << " base=[rbp-" << base_off << "] fields=" << n_fields << "\n";

        if (it != struct_layouts_.end()) {
            // Initialize in layout order so offsets are deterministic
            auto& names = it->second.field_names;
            for (int i = 0; i < static_cast<int>(names.size()); i++) {
                bool found = false;
                for (auto& f : si->fields) {
                    if (f.name == names[i]) {
                        gen_expr(f.value.get(), out);
                        out << "    mov [rbp-" << (base_off + i * 8) << "], rax"
                            << "    ; " << si->type_name << "." << names[i] << "\n";
                        found = true;
                        break;
                    }
                }
                if (!found)
                    out << "    mov qword [rbp-" << (base_off + i * 8)
                        << "], 0    ; " << names[i] << " (default 0)\n";
            }
        } else {
            // Unknown struct — initialize in the order provided
            for (int i = 0; i < static_cast<int>(si->fields.size()); i++) {
                gen_expr(si->fields[i].value.get(), out);
                out << "    mov [rbp-" << (base_off + i * 8) << "], rax"
                    << "    ; ." << si->fields[i].name << "\n";
            }
        }
    }

    // Generates a member-field read. Result in rax.
    void Backend::gen_member(const MemberExpr* mem, std::ostream& out) {
        auto* id = dynamic_cast<const IdentifierExpr*>(mem->object.get());
        if (!id) {
            out << "    ; TODO: complex member access\n";
            out << "    xor rax, rax\n";
            return;
        }

        auto type_it = var_types_.find(id->name);
        if (type_it == var_types_.end()) {
            out << "    ; unknown type for var " << id->name << "\n";
            out << "    xor rax, rax\n";
            return;
        }

        auto layout_it = struct_layouts_.find(type_it->second);
        if (layout_it == struct_layouts_.end()) {
            out << "    ; unknown struct layout " << type_it->second << "\n";
            out << "    xor rax, rax\n";
            return;
        }

        int field_idx = -1;
        auto& names = layout_it->second.field_names;
        for (int i = 0; i < static_cast<int>(names.size()); i++) {
            if (names[i] == mem->field) { field_idx = i; break; }
        }
        if (field_idx < 0) {
            out << "    ; unknown field " << mem->field
                << " in " << type_it->second << "\n";
            out << "    xor rax, rax\n";
            return;
        }

        int base_off = var_offsets_[id->name];
        out << "    mov rax, [rbp-" << (base_off + field_idx * 8) << "]"
            << "    ; " << id->name << "." << mem->field << "\n";
    }

    // Generates a member-field write. Value to store must already be in rax.
    void Backend::gen_assign_member(const MemberExpr* mem, std::ostream& out) {
        auto* id = dynamic_cast<const IdentifierExpr*>(mem->object.get());
        if (!id) { out << "    ; TODO: complex member assign\n"; return; }

        auto type_it = var_types_.find(id->name);
        if (type_it == var_types_.end()) {
            out << "    ; unknown type for var " << id->name << "\n"; return;
        }

        auto layout_it = struct_layouts_.find(type_it->second);
        if (layout_it == struct_layouts_.end()) {
            out << "    ; unknown struct layout " << type_it->second << "\n"; return;
        }

        int field_idx = -1;
        auto& names = layout_it->second.field_names;
        for (int i = 0; i < static_cast<int>(names.size()); i++) {
            if (names[i] == mem->field) { field_idx = i; break; }
        }
        if (field_idx < 0) {
            out << "    ; unknown field " << mem->field << "\n"; return;
        }

        int base_off = var_offsets_[id->name];
        out << "    mov [rbp-" << (base_off + field_idx * 8) << "], rax"
            << "    ; " << id->name << "." << mem->field << " =\n";
    }

    // ── var decl ─────────────────────────────────────────────────────

    void Backend::gen_var_decl(const VarDeclStmt* stmt, std::ostream& out) {
        // Struct init: allocates and initialises directly — no extra mov needed
        if (stmt->initializer) {
            if (auto* si = dynamic_cast<const StructInitExpr*>(stmt->initializer.get())) {
                if (!stmt->names.empty())
                    gen_struct_init(stmt->names[0], si, out);
                return;
            }
        }

        if (stmt->initializer)
            gen_expr(stmt->initializer.get(), out);
        else
            out << "    xor rax, rax\n"; // uninitialized — zero
        // rax now holds the value
        for (auto& name : stmt->names) {
            int off = alloc_stack_var(name);
            out << "    mov [rbp-" << off << "], rax\n";
            // Register type for println dispatch
            if (!stmt->type.empty())
                var_types_[name] = stmt->type;
            else if (stmt->initializer) {
                if (auto* lit = dynamic_cast<const LiteralExpr*>(stmt->initializer.get())) {
                    switch (lit->kind) {
                    case LiteralKind::Bool:   var_types_[name] = "bool";   break;
                    case LiteralKind::Float:  var_types_[name] = "f64";    break;
                    case LiteralKind::String: var_types_[name] = "string"; break;
                    default:                  var_types_[name] = "i32";    break;
                    }
                }
            }
        }
    }

    // ── assignment ───────────────────────────────────────────────────

    void Backend::gen_assign(const AssignStmt* stmt, std::ostream& out) {
        gen_expr(stmt->value.get(), out);

        // p.field = value
        if (auto* mem = dynamic_cast<const MemberExpr*>(stmt->target.get())) {
            gen_assign_member(mem, out);
            return;
        }

        // scalar assignment
        if (auto* id = dynamic_cast<const IdentifierExpr*>(stmt->target.get())) {
            auto it = var_offsets_.find(id->name);
            if (it != var_offsets_.end()) {
                if (stmt->op == "=") {
                    out << "    mov [rbp-" << it->second << "], rax\n";
                } else {
                    out << "    mov rbx, [rbp-" << it->second << "]\n";
                    if      (stmt->op == "+=") out << "    add rbx, rax\n";
                    else if (stmt->op == "-=") out << "    sub rbx, rax\n";
                    else if (stmt->op == "*=") out << "    imul rbx, rax\n";
                    out << "    mov [rbp-" << it->second << "], rbx\n";
                }
            }
        }
    }

    // ── return ───────────────────────────────────────────────────────

    void Backend::gen_return(const ReturnStmt* stmt, std::ostream& out) {
        if (stmt->value)
            gen_expr(stmt->value.get(), out);
        else
            out << "    xor rax, rax\n";
        emit_function_epilogue(out);
    }

    // ── if ───────────────────────────────────────────────────────────

    void Backend::gen_if(const IfStmt* stmt, std::ostream& out) {
        std::string else_lbl = new_label(".if_else");
        std::string end_lbl  = new_label(".if_end");

        gen_expr(stmt->condition.get(), out);
        out << "    test rax, rax\n";
        out << "    jz " << else_lbl << "\n";

        gen_block(stmt->then_block.get(), out);
        out << "    jmp " << end_lbl << "\n";

        out << else_lbl << ":\n";
        if (stmt->else_stmt)
            gen_stmt(stmt->else_stmt.get(), out);

        out << end_lbl << ":\n";
    }

    // ── while ────────────────────────────────────────────────────────

    void Backend::gen_while(const WhileStmt* stmt, std::ostream& out) {
        std::string loop_lbl = new_label(".while_loop");
        std::string end_lbl  = new_label(".while_end");

        out << loop_lbl << ":\n";
        gen_expr(stmt->condition.get(), out);
        out << "    test rax, rax\n";
        out << "    jz " << end_lbl << "\n";
        gen_block(stmt->body.get(), out);
        out << "    jmp " << loop_lbl << "\n";
        out << end_lbl << ":\n";
    }

    // ── for ──────────────────────────────────────────────────────────

    void Backend::gen_for(const ForStmt* stmt, std::ostream& out) {
        // Only handle range for now: for i in start..end
        auto* range = dynamic_cast<const RangeExpr*>(stmt->iterable.get());
        if (!range) {
            out << "    ; for (non-range iterator not yet implemented)\n";
            return;
        }

        std::string loop_lbl = new_label(".for_loop");
        std::string end_lbl  = new_label(".for_end");

        // Initialize counter
        gen_expr(range->start.get(), out);
        int i_off = alloc_stack_var(stmt->iterator);
        out << "    mov [rbp-" << i_off << "], rax\n";

        // Loop condition
        out << loop_lbl << ":\n";
        out << "    mov rax, [rbp-" << i_off << "]\n";
        gen_expr(range->end.get(), out);
        out << "    mov rbx, rax\n";
        out << "    mov rax, [rbp-" << i_off << "]\n";
        out << "    cmp rax, rbx\n";
        if (range->inclusive)
            out << "    jg " << end_lbl << "\n";
        else
            out << "    jge " << end_lbl << "\n";

        gen_block(stmt->body.get(), out);

        // Increment
        out << "    mov rax, [rbp-" << i_off << "]\n";
        out << "    inc rax\n";
        out << "    mov [rbp-" << i_off << "], rax\n";
        out << "    jmp " << loop_lbl << "\n";
        out << end_lbl << ":\n";
    }

    // ── expression statement ─────────────────────────────────────────

    void Backend::gen_expr_stmt(const ExpressionStmt* stmt, std::ostream& out) {
        gen_expr(stmt->expression.get(), out);
    }

    // ================================================================
    // Expression generation
    // Result always in rax
    // ================================================================

    void Backend::gen_expr(const Expr* expr, std::ostream& out) {
        if (!expr) {
            out << "    xor rax, rax\n";
            return;
        }

        if (auto* e = dynamic_cast<const LiteralExpr*>(expr))
        { gen_literal(e, out); return; }

        if (auto* e = dynamic_cast<const IdentifierExpr*>(expr))
        { gen_identifier(e, out); return; }

        if (auto* e = dynamic_cast<const BinaryExpr*>(expr))
        { gen_binary(e, out); return; }

        if (auto* e = dynamic_cast<const CallExpr*>(expr))
        { gen_call(e, out); return; }

        if (auto* e = dynamic_cast<const UnaryExpr*>(expr)) {
            gen_expr(e->operand.get(), out);
            if      (e->op == "-") out << "    neg rax\n";
            else if (e->op == "!") out << "    xor rax, 1\n";
            else if (e->op == "~") out << "    not rax\n";
            return;
        }

        if (auto* e = dynamic_cast<const MemberExpr*>(expr)) {
            gen_member(e, out);
            return;
        }

        if (auto* e = dynamic_cast<const StructInitExpr*>(expr)) {
            // StructInitExpr as expression (not inside let) — allocate anonymous slot
            gen_struct_init("__anon_struct_" + std::to_string(label_counter_++), e, out);
            return;
        }

        if (auto* e = dynamic_cast<const IndexExpr*>(expr)) {
            gen_expr(e->index.get(), out);
            out << "    mov rbx, rax\n";
            gen_expr(e->object.get(), out);
            out << "    ; array[rbx] (TODO: bounds check)\n";
            return;
        }

        if (auto* e = dynamic_cast<const MoveExpr*>(expr)) {
            gen_expr(e->expr.get(), out);
            out << "    ; move (ownership transferred)\n";
            return;
        }

        if (auto* e = dynamic_cast<const OkExpr*>(expr)) {
            gen_expr(e->expr.get(), out);
            return;
        }

        if (auto* e = dynamic_cast<const SomeExpr*>(expr)) {
            gen_expr(e->expr.get(), out);
            return;
        }

        if (dynamic_cast<const NoneExpr*>(expr)) {
            out << "    xor rax, rax\n";
            return;
        }

        if (auto* e = dynamic_cast<const RangeExpr*>(expr)) {
            gen_expr(e->start.get(), out);
            return;
        }

        // Interpolated string — for now treat as regular string
        if (auto* e = dynamic_cast<const InterpolatedStringExpr*>(expr)) {
            // Collect all text parts
            std::string combined;
            for (auto& part : e->parts) {
                if (!part.is_expr)
                    combined += part.raw_text;
                else
                    combined += "{...}"; // TODO: evaluate embedded expressions
            }
            std::string label = new_string_label(combined);
            out << "    mov rax, " << label << "\n";
            return;
        }

        out << "    xor rax, rax ; unhandled expr\n";
    }

    // ── Literal ──────────────────────────────────────────────────────

    void Backend::gen_literal(const LiteralExpr* expr, std::ostream& out) {
        switch (expr->kind) {
            case LiteralKind::Int:
                out << "    mov rax, " << expr->value << "\n";
                break;

            case LiteralKind::Float:
                // For now load as integer approximation
                // TODO: use xmm registers for float
                out << "    ; float literal " << expr->value << " (TODO: xmm)\n";
                out << "    xor rax, rax\n";
                break;

            case LiteralKind::String: {
                std::string label = new_string_label(expr->value);
                out << "    mov rax, " << label << "\n";
                break;
            }

            case LiteralKind::Bool:
                out << "    mov rax, " << (expr->value == "true" ? 1 : 0) << "\n";
                break;
        }
    }

    // ── Identifier ───────────────────────────────────────────────────

    void Backend::gen_identifier(const IdentifierExpr* expr, std::ostream& out) {
        auto it = var_offsets_.find(expr->name);
        if (it != var_offsets_.end()) {
            out << "    mov rax, [rbp-" << it->second << "]\n";
        } else {
            // Global or function name
            out << "    mov rax, " << expr->name << "\n";
        }
    }

    // ── Binary expression ─────────────────────────────────────────────

    void Backend::gen_binary(const BinaryExpr* expr, std::ostream& out) {
        gen_expr(expr->left.get(), out);
        out << "    push rax\n";
        gen_expr(expr->right.get(), out);
        out << "    mov rbx, rax\n";
        out << "    pop rax\n";

        if      (expr->op == "+")  out << "    add rax, rbx\n";
        else if (expr->op == "-")  out << "    sub rax, rbx\n";
        else if (expr->op == "*")  out << "    imul rax, rbx\n";
        else if (expr->op == "/") {
            out << "    xor rdx, rdx\n";
            out << "    div rbx\n";
        }
        else if (expr->op == "%") {
            out << "    xor rdx, rdx\n";
            out << "    div rbx\n";
            out << "    mov rax, rdx\n";
        }
        else if (expr->op == "&")  out << "    and rax, rbx\n";
        else if (expr->op == "|")  out << "    or  rax, rbx\n";
        else if (expr->op == "^")  out << "    xor rax, rbx\n";
        else if (expr->op == "<<") out << "    shl rax, cl\n";
        else if (expr->op == ">>") out << "    shr rax, cl\n";
        else if (expr->op == "==") {
            out << "    cmp rax, rbx\n";
            out << "    sete al\n";
            out << "    movzx rax, al\n";
        }
        else if (expr->op == "!=") {
            out << "    cmp rax, rbx\n";
            out << "    setne al\n";
            out << "    movzx rax, al\n";
        }
        else if (expr->op == "<") {
            out << "    cmp rax, rbx\n";
            out << "    setl al\n";
            out << "    movzx rax, al\n";
        }
        else if (expr->op == ">") {
            out << "    cmp rax, rbx\n";
            out << "    setg al\n";
            out << "    movzx rax, al\n";
        }
        else if (expr->op == "<=") {
            out << "    cmp rax, rbx\n";
            out << "    setle al\n";
            out << "    movzx rax, al\n";
        }
        else if (expr->op == ">=") {
            out << "    cmp rax, rbx\n";
            out << "    setge al\n";
            out << "    movzx rax, al\n";
        }
        else if (expr->op == "&&") {
            out << "    test rax, rax\n";
            out << "    setnz al\n";
            out << "    test rbx, rbx\n";
            out << "    setnz bl\n";
            out << "    and al, bl\n";
            out << "    movzx rax, al\n";
        }
        else if (expr->op == "||") {
            out << "    or rax, rbx\n";
            out << "    setnz al\n";
            out << "    movzx rax, al\n";
        }
        else {
            out << "    ; unhandled op " << expr->op << "\n";
        }
    }

    // ── Call expression ───────────────────────────────────────────────

    void Backend::gen_call(const CallExpr* expr, std::ostream& out) {
        // Check if it's println or print
        if (auto* id = dynamic_cast<const IdentifierExpr*>(expr->callee.get())) {
            if (id->name == "println" || id->name == "print") {
                gen_println_call(expr, out);
                return;
            }
        }

        // Regular function call — System V ABI
        static const char* arg_regs[] = {"rdi","rsi","rdx","rcx","r8","r9"};
        int arg_count = (int)expr->arguments.size();

        // Evaluate arguments and push to stack (right to left)
        for (int i = arg_count - 1; i >= 0; i--) {
            gen_expr(expr->arguments[i].get(), out);
            out << "    push rax\n";
        }

        // Pop into registers (left to right)
        for (int i = 0; i < arg_count && i < 6; i++) {
            out << "    pop " << arg_regs[i] << "\n";
        }

        // Call
        gen_expr(expr->callee.get(), out);
        out << "    call rax\n";
    }

    // ── println call ─────────────────────────────────────────────────

    void Backend::gen_println_call(const CallExpr* expr, std::ostream& out) {
        if (expr->arguments.empty()) {
            // println() — just print newline
            out << "    mov rax, 1\n";
            out << "    mov rdi, 1\n";
            out << "    mov rsi, sysp_newline\n";
            out << "    mov rdx, 1\n";
            out << "    syscall\n";
            return;
        }

        auto* arg = expr->arguments[0].get();

        // String literal → direct syscall write
        if (auto* lit = dynamic_cast<const LiteralExpr*>(arg)) {
            if (lit->kind == LiteralKind::String) {
                std::string label = new_string_label(lit->value);
                out << "    mov rax, 1\n";            // sys_write
                out << "    mov rdi, 1\n";            // stdout
                out << "    mov rsi, " << label << "\n";
                out << "    mov rdx, " << label << "_len\n";
                out << "    syscall\n";
                return;
            }
            if (lit->kind == LiteralKind::Int) {
                out << "    mov rdi, " << lit->value << "\n";
                out << "    call sysp_println_int\n";
                return;
            }
            if (lit->kind == LiteralKind::Bool) {
                out << "    mov rdi, " << (lit->value == "true" ? 1 : 0) << "\n";
                out << "    call sysp_println_bool\n";
                return;
            }
        }

        // Interpolated string
        if (auto* interp = dynamic_cast<const InterpolatedStringExpr*>(arg)) {
            std::string combined;
            for (auto& part : interp->parts) {
                if (!part.is_expr) combined += part.raw_text;
                else combined += "{}";
            }
            std::string label = new_string_label(combined);
            out << "    mov rax, 1\n";
            out << "    mov rdi, 1\n";
            out << "    mov rsi, " << label << "\n";
            out << "    mov rdx, " << label << "_len\n";
            out << "    syscall\n";
            return;
        }

        // Variable or expression → evaluate and dispatch
        gen_expr(arg, out);
        out << "    mov rdi, rax\n";

        // Detect type to call correct println variant
        // Check if argument is an identifier with known type
        if (auto* id = dynamic_cast<const IdentifierExpr*>(arg)) {
            // Look up variable type from type map
            auto it = var_types_.find(id->name);
            if (it != var_types_.end()) {
                if (it->second == "bool") {
                    out << "    call sysp_println_bool\n";
                    return;
                }
                if (it->second == "f32" || it->second == "f64") {
                    out << "    call sysp_println_int\n"; // TODO: float
                    return;
                }
                if (it->second == "string") {
                    // rax has the string address — print as string
                    out << "    mov rsi, rdi\n";
                    // compute length
                    out << "    ; TODO: string variable println\n";
                    out << "    call sysp_println_int\n";
                    return;
                }
            }
        }

        // Member access: println(p.field) — look up field type for dispatch
        if (auto* mem = dynamic_cast<const MemberExpr*>(arg)) {
            if (auto* id = dynamic_cast<const IdentifierExpr*>(mem->object.get())) {
                auto type_it = var_types_.find(id->name);
                if (type_it != var_types_.end()) {
                    auto layout_it = struct_layouts_.find(type_it->second);
                    if (layout_it != struct_layouts_.end()) {
                        auto& names = layout_it->second.field_names;
                        auto& types = layout_it->second.field_types;
                        for (size_t i = 0; i < names.size(); i++) {
                            if (names[i] == mem->field) {
                                if (types[i] == "bool") {
                                    out << "    call sysp_println_bool\n";
                                    return;
                                }
                                break;
                            }
                        }
                    }
                }
            }
            out << "    call sysp_println_int\n";
            return;
        }

        // Default: print as int
        out << "    call sysp_println_int\n";
    }

} // namespace sysp::backend::x86
