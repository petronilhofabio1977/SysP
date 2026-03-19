#include "parser.hpp"
#include <sstream>
#include <unordered_map>

namespace sysp::parser {

    using namespace sysp::ast;

    // ================================================================
    // Token navigation
    // ================================================================

    Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

    const Token& Parser::current() const {
        return tokens_[pos_];
    }

    const Token& Parser::peek(int offset) const {
        size_t idx = pos_ + static_cast<size_t>(offset);
        if (idx >= tokens_.size()) return tokens_.back();
        return tokens_[idx];
    }

    Token Parser::advance() {
        Token t = tokens_[pos_];
        if (!is_at_end()) pos_++;
        return t;
    }

    bool Parser::check(TokenType t) const {
        return current().type == t;
    }

    bool Parser::check_any(std::initializer_list<TokenType> types) const {
        for (auto t : types) {
            if (check(t)) return true;
        }
        return false;
    }

    bool Parser::match(TokenType t) {
        if (!check(t)) return false;
        advance();
        return true;
    }

    bool Parser::match_any(std::initializer_list<TokenType> types) {
        for (auto t : types) {
            if (match(t)) return true;
        }
        return false;
    }

    Token Parser::expect(TokenType t, const std::string& msg) {
        if (!check(t)) throw error(msg + " (got '" + current().lexeme + "')");
        return advance();
    }

    bool Parser::is_at_end() const {
        return current().type == TokenType::END;
    }

    ParseError Parser::error(const std::string& msg) const {
        return ParseError(msg, current().line, current().column);
    }

    // ================================================================
    // Program
    // ================================================================

    Program Parser::parse_program() {
        Program prog;

        // module declarations
        while (check(TokenType::MODULE))
            prog.modules.push_back(parse_module_decl());

        // top-level declarations
        while (!is_at_end())
            prog.declarations.push_back(parse_declaration());

        return prog;
    }

    // ================================================================
    // Module declaration
    // ================================================================

    std::unique_ptr<ModuleDecl> Parser::parse_module_decl() {
        expect(TokenType::MODULE, "expected 'module'");
        auto decl = std::make_unique<ModuleDecl>();

        // path: identifier { . identifier }
        std::string path = expect(TokenType::IDENT, "expected module path").lexeme;
        while (match(TokenType::DOT))
            path += "." + expect(TokenType::IDENT, "expected identifier after '.'").lexeme;

        decl->path = path;
        return decl;
    }

    // ================================================================
    // Top-level declarations
    // ================================================================

    DeclPtr Parser::parse_declaration() {
        bool is_pub = match(TokenType::PUB);

        if (check(TokenType::FN))         return parse_function(is_pub);
        if (check(TokenType::STRUCT))     return parse_struct(is_pub);
        if (check(TokenType::ENUM))       return parse_enum(is_pub);
        if (check(TokenType::TRAIT))      { if (is_pub) throw error("trait cannot be pub"); return parse_trait(); }
        if (check(TokenType::IMPL))       { if (is_pub) throw error("impl cannot be pub"); return parse_impl(); }
        if (check(TokenType::TYPE))       return parse_type_alias(is_pub);
        if (check(TokenType::CONST))      return parse_const_decl(is_pub);

        // var declarations at top level — should not reach here normally
        throw error("expected declaration (fn, struct, enum, trait, impl, type, const)");
    }

    // ================================================================
    // Generic params and where clause
    // ================================================================

    // <T>   <T: Ord>   <T: Ord + Display, U>
    std::vector<GenericParam> Parser::parse_generic_params() {
        std::vector<GenericParam> params;
        if (!match(TokenType::LT)) return params;

        do {
            GenericParam gp;
            gp.name = expect(TokenType::IDENT, "expected type parameter name").lexeme;
            if (match(TokenType::COLON))
                gp.bounds = parse_trait_bound();
            params.push_back(std::move(gp));
        } while (match(TokenType::COMMA));

        expect(TokenType::GT, "expected '>' after generic params");
        return params;
    }

    // Trait1 + Trait2 + ...
    std::vector<std::string> Parser::parse_trait_bound() {
        std::vector<std::string> bounds;
        bounds.push_back(expect(TokenType::IDENT, "expected trait name").lexeme);
        while (match(TokenType::PLUS))
            bounds.push_back(expect(TokenType::IDENT, "expected trait name after '+'").lexeme);
        return bounds;
    }

    // where T: Ord, U: Display
    std::vector<WhereItem> Parser::parse_where_clause() {
        std::vector<WhereItem> items;
        if (!match(TokenType::WHERE)) return items;

        do {
            WhereItem wi;
            wi.type_name = expect(TokenType::IDENT, "expected type in where clause").lexeme;
            expect(TokenType::COLON, "expected ':' in where clause");
            wi.bounds = parse_trait_bound();
            items.push_back(std::move(wi));
        } while (match(TokenType::COMMA) && check(TokenType::IDENT));

        return items;
    }

    // ================================================================
    // Type parsing
    // ================================================================

    // Returns type as string for simplicity (full type AST can come later)
    std::string Parser::parse_type() {
        // Pointer: *T
        if (match(TokenType::STAR)) return "*" + parse_type();

        // Reference: ref T
        if (match(TokenType::REF)) return "ref " + parse_type();

        // Array: [N]T
        if (check(TokenType::LBRACKET)) {
            advance(); // [
            if (match(TokenType::RBRACKET)) {
                // slice []T
                return "[]" + parse_type();
            }
            // fixed array [N]T
            auto n = expect(TokenType::INTEGER, "expected array size").lexeme;
            expect(TokenType::RBRACKET, "expected ']'");
            return "[" + n + "]" + parse_type();
        }

        // Tuple: (T, U)
        if (check(TokenType::LPAREN)) {
            advance();
            std::string t = "(" + parse_type();
            while (match(TokenType::COMMA)) t += ", " + parse_type();
            expect(TokenType::RPAREN, "expected ')' after tuple type");
            return t + ")";
        }

        // Lambda type: fn(T, U) -> V
        if (check(TokenType::FN)) {
            advance();
            expect(TokenType::LPAREN, "expected '('");
            std::string sig = "fn(";
            bool first = true;
            while (!check(TokenType::RPAREN)) {
                if (!first) sig += ", ";
                sig += parse_type();
                first = false;
                if (!check(TokenType::RPAREN)) expect(TokenType::COMMA, "expected ','");
            }
            advance(); // )
            sig += ")";
            if (match(TokenType::ARROW)) sig += " -> " + parse_type();
            return sig;
        }

        // Named type (possibly generic)
        std::string name;
        // primitive types
        static const std::unordered_map<TokenType, std::string> PRIM = {
            {TokenType::KW_I8,"i8"},{TokenType::KW_I16,"i16"},
            {TokenType::KW_I32,"i32"},{TokenType::KW_I64,"i64"},
            {TokenType::KW_U8,"u8"},{TokenType::KW_U16,"u16"},
            {TokenType::KW_U32,"u32"},{TokenType::KW_U64,"u64"},
            {TokenType::KW_F32,"f32"},{TokenType::KW_F64,"f64"},
            {TokenType::KW_BOOL,"bool"},{TokenType::KW_STRING,"string"},
            {TokenType::KW_RESULT,"Result"},{TokenType::KW_OPTION,"Option"},
            {TokenType::KW_TASK,"Task"},{TokenType::KW_CHANNEL_TYPE,"Channel"},
        };
        auto it = PRIM.find(current().type);
        if (it != PRIM.end()) {
            name = it->second;
            advance();
        } else {
            name = expect(TokenType::IDENT, "expected type name").lexeme;
        }

        // Generic args: Name<T, U>
        if (check(TokenType::LT)) {
            advance();
            name += "<" + parse_type();
            while (match(TokenType::COMMA)) name += ", " + parse_type();
            expect(TokenType::GT, "expected '>' after type arguments");
            name += ">";
        }

        return name;
    }

    // ================================================================
    // Parameters
    // ================================================================

    std::vector<Parameter> Parser::parse_parameter_list() {
        std::vector<Parameter> params;
        if (check(TokenType::RPAREN)) return params;

        params.push_back(parse_parameter());
        while (match(TokenType::COMMA) && !check(TokenType::RPAREN))
            params.push_back(parse_parameter());

        return params;
    }

    Parameter Parser::parse_parameter() {
        Parameter p;

        // self
        if (match(TokenType::SELF)) {
            p.is_self = true;
            p.name    = "self";
            return p;
        }

        // name [ref] type
        p.name = expect(TokenType::IDENT, "expected parameter name").lexeme;
        if (match(TokenType::REF)) p.is_ref = true;
        p.type = parse_type();
        return p;
    }

    // ================================================================
    // Function declaration
    // ================================================================

    std::unique_ptr<FunctionDecl> Parser::parse_function(bool is_pub) {
        expect(TokenType::FN, "expected 'fn'");
        auto decl     = std::make_unique<FunctionDecl>();
        decl->is_pub  = is_pub;
        decl->name    = expect(TokenType::IDENT, "expected function name").lexeme;
        decl->generics = parse_generic_params();

        expect(TokenType::LPAREN, "expected '('");
        decl->parameters = parse_parameter_list();
        expect(TokenType::RPAREN, "expected ')'");

        if (match(TokenType::ARROW))
            decl->return_type = parse_type();

        decl->where_clause = parse_where_clause();
        decl->body         = parse_block();
        return decl;
    }

    // ================================================================
    // Struct declaration
    // ================================================================

    std::unique_ptr<StructDecl> Parser::parse_struct(bool is_pub) {
        expect(TokenType::STRUCT, "expected 'struct'");
        auto decl    = std::make_unique<StructDecl>();
        decl->is_pub = is_pub;
        decl->name   = expect(TokenType::IDENT, "expected struct name").lexeme;
        decl->generics     = parse_generic_params();
        decl->where_clause = parse_where_clause();

        expect(TokenType::LBRACE, "expected '{'");
        while (!check(TokenType::RBRACE) && !is_at_end()) {
            StructField f;
            f.is_pub = match(TokenType::PUB);
            f.name   = expect(TokenType::IDENT, "expected field name").lexeme;
            f.type   = parse_type();
            decl->fields.push_back(std::move(f));
        }
        expect(TokenType::RBRACE, "expected '}'");
        return decl;
    }

    // ================================================================
    // Enum declaration
    // ================================================================

    std::unique_ptr<EnumDecl> Parser::parse_enum(bool is_pub) {
        expect(TokenType::ENUM, "expected 'enum'");
        auto decl    = std::make_unique<EnumDecl>();
        decl->is_pub = is_pub;
        decl->name   = expect(TokenType::IDENT, "expected enum name").lexeme;
        decl->generics = parse_generic_params();

        expect(TokenType::LBRACE, "expected '{'");
        do {
            if (check(TokenType::RBRACE)) break;
            EnumVariant v;
            v.name = expect(TokenType::IDENT, "expected variant name").lexeme;
            if (match(TokenType::LPAREN)) {
                v.fields.push_back(parse_type());
                while (match(TokenType::COMMA)) v.fields.push_back(parse_type());
                expect(TokenType::RPAREN, "expected ')'");
            }
            decl->variants.push_back(std::move(v));
        } while (match(TokenType::COMMA));
        expect(TokenType::RBRACE, "expected '}'");
        return decl;
    }

    // ================================================================
    // Trait declaration
    // ================================================================

    std::unique_ptr<TraitDecl> Parser::parse_trait() {
        expect(TokenType::TRAIT, "expected 'trait'");
        auto decl       = std::make_unique<TraitDecl>();
        decl->name      = expect(TokenType::IDENT, "expected trait name").lexeme;
        decl->generics  = parse_generic_params();
        decl->where_clause = parse_where_clause();

        expect(TokenType::LBRACE, "expected '{'");
        while (!check(TokenType::RBRACE) && !is_at_end()) {
            if (check(TokenType::TYPE)) {
                advance();
                AssocType at;
                at.name = expect(TokenType::IDENT, "expected associated type name").lexeme;
                decl->assoc_types.push_back(std::move(at));
                continue;
            }

            expect(TokenType::FN, "expected 'fn' in trait");
            TraitMethod m;
            m.name     = expect(TokenType::IDENT, "expected method name").lexeme;
            m.generics = parse_generic_params();
            expect(TokenType::LPAREN, "expected '('");
            m.parameters = parse_parameter_list();
            expect(TokenType::RPAREN, "expected ')'");
            if (match(TokenType::ARROW)) m.return_type = parse_type();
            m.where_clause = parse_where_clause();

            if (check(TokenType::LBRACE)) {
                m.has_default  = true;
                m.default_body = parse_block();
            }
            decl->methods.push_back(std::move(m));
        }
        expect(TokenType::RBRACE, "expected '}'");
        return decl;
    }

    // ================================================================
    // Impl declaration
    // ================================================================

    DeclPtr Parser::parse_impl() {
        expect(TokenType::IMPL, "expected 'impl'");
        std::string first_name = expect(TokenType::IDENT, "expected type name").lexeme;
        auto generics = parse_generic_params();

        // impl Trait for Type
        if (check(TokenType::FOR))
            return parse_impl_trait(first_name, generics);

        auto decl          = std::make_unique<ImplDecl>();
        decl->type_name    = first_name;
        decl->generics     = generics;
        decl->where_clause = parse_where_clause();

        expect(TokenType::LBRACE, "expected '{'");
        while (!check(TokenType::RBRACE) && !is_at_end()) {
            bool is_pub = match(TokenType::PUB);
            decl->methods.push_back(parse_function(is_pub));
        }
        expect(TokenType::RBRACE, "expected '}'");
        return decl;
    }

    std::unique_ptr<ImplTraitDecl> Parser::parse_impl_trait(
        const std::string& first_name,
        const std::vector<GenericParam>& first_generics)
    {
        expect(TokenType::FOR, "expected 'for'");
        auto decl             = std::make_unique<ImplTraitDecl>();
        decl->trait_name      = first_name;
        decl->trait_generics  = first_generics;
        decl->type_name       = expect(TokenType::IDENT, "expected type name").lexeme;
        decl->type_generics   = parse_generic_params();
        decl->where_clause    = parse_where_clause();

        expect(TokenType::LBRACE, "expected '{'");
        while (!check(TokenType::RBRACE) && !is_at_end()) {
            bool is_pub = match(TokenType::PUB);
            decl->methods.push_back(parse_function(is_pub));
        }
        expect(TokenType::RBRACE, "expected '}'");
        return decl;
    }

    // ================================================================
    // Type alias
    // ================================================================

    std::unique_ptr<TypeAliasDecl> Parser::parse_type_alias(bool is_pub) {
        expect(TokenType::TYPE, "expected 'type'");
        auto decl    = std::make_unique<TypeAliasDecl>();
        decl->is_pub = is_pub;
        decl->name   = expect(TokenType::IDENT, "expected type alias name").lexeme;
        expect(TokenType::EQ, "expected '='");
        decl->type   = parse_type();
        return decl;
    }

    // ================================================================
    // Const decl at top level
    // ================================================================

    std::unique_ptr<ConstDecl> Parser::parse_const_decl(bool is_pub) {
        expect(TokenType::CONST, "expected 'const'");
        auto decl    = std::make_unique<ConstDecl>();
        decl->is_pub = is_pub;
        decl->name   = expect(TokenType::IDENT, "expected constant name").lexeme;
        if (match(TokenType::COLON)) decl->type = parse_type();
        expect(TokenType::EQ, "expected '='");
        decl->value  = parse_expression();
        return decl;
    }

    // ================================================================
    // Block
    // ================================================================

    std::unique_ptr<BlockStmt> Parser::parse_block() {
        expect(TokenType::LBRACE, "expected '{'");
        auto block = std::make_unique<BlockStmt>();

        while (!check(TokenType::RBRACE) && !is_at_end())
            block->statements.push_back(parse_statement());

        expect(TokenType::RBRACE, "expected '}'");
        return block;
    }

    // ================================================================
    // Statements
    // ================================================================

    StmtPtr Parser::parse_statement() {
        if (check(TokenType::LET))      return parse_var_decl();
        if (check(TokenType::CONST))    return parse_const_stmt();
        if (check(TokenType::IF))       return parse_if();
        if (check(TokenType::WHILE))    return parse_while();
        if (check(TokenType::LOOP))     return parse_loop();
        if (check(TokenType::FOR))      return parse_for();
        if (check(TokenType::MATCH))    return parse_match();
        if (check(TokenType::RETURN))   return parse_return();
        if (check(TokenType::BREAK))    return parse_break();
        if (check(TokenType::CONTINUE)) { advance(); return std::make_unique<ContinueStmt>(); }
        if (check(TokenType::REGION))   return parse_region();
        if (check(TokenType::UNSAFE))   return parse_unsafe();
        if (check(TokenType::DROP))     return parse_drop_stmt();
        if (check(TokenType::PANIC))    return parse_panic();
        if (check(TokenType::SPAWN))    return parse_spawn();
        if (check(TokenType::SEND))     return parse_send();
        if (check(TokenType::SELECT))   return parse_select();

        // var decl without let: x: i32 = expr
        if (check(TokenType::IDENT) && peek(1).type == TokenType::COLON)
            return parse_var_decl();

        // expression or assignment
        auto expr = parse_expression();

        // assignment: expr op= rhs
        if (is_assignment_op(current().type)) {
            std::string op = advance().lexeme;
            auto rhs = parse_expression();
            auto stmt = std::make_unique<AssignStmt>();
            stmt->target = std::move(expr);
            stmt->op     = op;
            stmt->value  = std::move(rhs);
            return stmt;
        }

        auto stmt = std::make_unique<ExpressionStmt>();
        stmt->expression = std::move(expr);
        return stmt;
    }

    // ── let / typed var decl ─────────────────────────────────────────

    StmtPtr Parser::parse_var_decl() {
        auto stmt = std::make_unique<VarDeclStmt>();

        if (match(TokenType::LET)) {
            stmt->is_let = true;
            // let (a, b) = expr  — tuple destructuring
            if (match(TokenType::LPAREN)) {
                stmt->is_tuple_destructure = true;
                stmt->names.push_back(expect(TokenType::IDENT, "expected name").lexeme);
                while (match(TokenType::COMMA))
                    stmt->names.push_back(expect(TokenType::IDENT, "expected name").lexeme);
                expect(TokenType::RPAREN, "expected ')'");
            } else {
                stmt->names.push_back(expect(TokenType::IDENT, "expected variable name").lexeme);
            }
            // optional type annotation: let x: i32 = ...
            if (match(TokenType::COLON)) stmt->type = parse_type();
        } else {
            // x: Type = expr
            stmt->is_let = false;
            stmt->names.push_back(expect(TokenType::IDENT, "expected variable name").lexeme);
            expect(TokenType::COLON, "expected ':'");
            stmt->type = parse_type();
        }

        expect(TokenType::EQ, "expected '='");
        stmt->initializer = parse_expression();
        return stmt;
    }

    // ── const in statement context ───────────────────────────────────

    StmtPtr Parser::parse_const_stmt() {
        expect(TokenType::CONST, "expected 'const'");
        auto stmt    = std::make_unique<ConstDeclStmt>();
        stmt->is_pub = false;
        stmt->name   = expect(TokenType::IDENT, "expected constant name").lexeme;
        if (match(TokenType::COLON)) stmt->type = parse_type();
        expect(TokenType::EQ, "expected '='");
        stmt->value  = parse_expression();
        return stmt;
    }

    // ── if ──────────────────────────────────────────────────────────

    StmtPtr Parser::parse_if() {
        expect(TokenType::IF, "expected 'if'");
        auto stmt       = std::make_unique<IfStmt>();
        stmt->condition = parse_expression();
        stmt->then_block = parse_block();

        if (match(TokenType::ELSE)) {
            if (check(TokenType::IF))
                stmt->else_stmt = parse_if();
            else {
                auto else_block = parse_block();
                stmt->else_stmt = std::move(else_block);
            }
        }
        return stmt;
    }

    // ── while ───────────────────────────────────────────────────────

    StmtPtr Parser::parse_while() {
        expect(TokenType::WHILE, "expected 'while'");
        auto stmt       = std::make_unique<WhileStmt>();
        stmt->condition = parse_expression();
        stmt->body      = parse_block();
        return stmt;
    }

    // ── loop ────────────────────────────────────────────────────────

    StmtPtr Parser::parse_loop() {
        expect(TokenType::LOOP, "expected 'loop'");
        auto stmt = std::make_unique<LoopStmt>();
        stmt->body = parse_block();
        return stmt;
    }

    // ── for ─────────────────────────────────────────────────────────

    StmtPtr Parser::parse_for() {
        expect(TokenType::FOR, "expected 'for'");
        auto stmt    = std::make_unique<ForStmt>();
        stmt->iterator = expect(TokenType::IDENT, "expected iterator name").lexeme;
        expect(TokenType::IN, "expected 'in'");
        stmt->iterable = parse_expression();
        stmt->body     = parse_block();
        return stmt;
    }

    // ── match ───────────────────────────────────────────────────────

    StmtPtr Parser::parse_match() {
        expect(TokenType::MATCH, "expected 'match'");
        auto stmt  = std::make_unique<MatchStmt>();
        stmt->value = parse_expression();

        expect(TokenType::LBRACE, "expected '{'");
        while (!check(TokenType::RBRACE) && !is_at_end()) {
            MatchArm arm;
            arm.pattern = parse_pattern();

            // optional guard: if expr
            if (match(TokenType::IF))
                arm.guard = parse_expression();

            expect(TokenType::FAT_ARROW, "expected '=>'");

            if (check(TokenType::LBRACE)) {
                arm.body = parse_block();
            } else {
                auto expr_stmt = std::make_unique<ExpressionStmt>();
                expr_stmt->expression = parse_expression();
                arm.body = std::move(expr_stmt);
            }

            stmt->arms.push_back(std::move(arm));
        }
        expect(TokenType::RBRACE, "expected '}'");
        return stmt;
    }

    // ── Pattern parsing ─────────────────────────────────────────────

    std::unique_ptr<Pattern> Parser::parse_pattern() {
        auto p = std::make_unique<Pattern>();

        // Wildcard: _
        if (match(TokenType::UNDERSCORE)) {
            p->kind = PatternKind::Wildcard;
            return p;
        }

        // None
        if (match(TokenType::NONE)) {
            p->kind = PatternKind::None;
            p->name = "None";
            return p;
        }

        // Some(pattern)
        if (match(TokenType::SOME)) {
            p->kind = PatternKind::Some;
            expect(TokenType::LPAREN, "expected '(' after Some");
            p->sub_patterns.push_back(parse_pattern());
            expect(TokenType::RPAREN, "expected ')'");
            return p;
        }

        // Ok(pattern)
        if (match(TokenType::OK)) {
            p->kind = PatternKind::Ok;
            expect(TokenType::LPAREN, "expected '(' after Ok");
            p->sub_patterns.push_back(parse_pattern());
            expect(TokenType::RPAREN, "expected ')'");
            return p;
        }

        // Err(pattern)
        if (match(TokenType::ERR)) {
            p->kind = PatternKind::Err;
            expect(TokenType::LPAREN, "expected '(' after Err");
            p->sub_patterns.push_back(parse_pattern());
            expect(TokenType::RPAREN, "expected ')'");
            return p;
        }

        // Tuple pattern: (a, b)
        if (match(TokenType::LPAREN)) {
            p->kind = PatternKind::Tuple;
            p->sub_patterns.push_back(parse_pattern());
            while (match(TokenType::COMMA))
                p->sub_patterns.push_back(parse_pattern());
            expect(TokenType::RPAREN, "expected ')'");
            return p;
        }

        // Integer literal or range: 1   1..10   1..=10
        if (check(TokenType::INTEGER)) {
            std::string low = advance().lexeme;
            if (check(TokenType::DOT_DOT) || check(TokenType::DOT_DOT_EQ)) {
                p->kind            = PatternKind::Range;
                p->range_inclusive = match(TokenType::DOT_DOT_EQ);
                if (!p->range_inclusive) advance(); // ..
                p->range_low  = low;
                p->range_high = expect(TokenType::INTEGER, "expected upper bound").lexeme;
            } else {
                p->kind = PatternKind::Literal;
                p->name = low;
            }
            return p;
        }

        // String / bool literal
        if (check(TokenType::STRING) || check(TokenType::BOOL_TRUE) || check(TokenType::BOOL_FALSE)) {
            p->kind = PatternKind::Literal;
            p->name = advance().lexeme;
            return p;
        }

        // Identifier or EnumVariant(...)
        if (check(TokenType::IDENT)) {
            std::string name = advance().lexeme;
            if (match(TokenType::LPAREN)) {
                // EnumVariant(sub_patterns...)
                p->kind         = PatternKind::EnumVariant;
                p->variant_name = name;
                p->sub_patterns.push_back(parse_pattern());
                while (match(TokenType::COMMA))
                    p->sub_patterns.push_back(parse_pattern());
                expect(TokenType::RPAREN, "expected ')'");
            } else {
                p->kind = PatternKind::Identifier;
                p->name = name;
            }
            return p;
        }

        throw error("expected pattern");
    }

    // ── return ──────────────────────────────────────────────────────

    StmtPtr Parser::parse_return() {
        expect(TokenType::RETURN, "expected 'return'");
        auto stmt = std::make_unique<ReturnStmt>();
        if (!check(TokenType::RBRACE) && !is_at_end())
            stmt->value = parse_expression();
        return stmt;
    }

    // ── break ───────────────────────────────────────────────────────

    StmtPtr Parser::parse_break() {
        expect(TokenType::BREAK, "expected 'break'");
        auto stmt = std::make_unique<BreakStmt>();
        // optional value: break expr
        if (!check(TokenType::RBRACE) && !check(TokenType::COMMA) && !is_at_end())
            stmt->value = parse_expression();
        return stmt;
    }

    // ── region ──────────────────────────────────────────────────────

    StmtPtr Parser::parse_region() {
        expect(TokenType::REGION, "expected 'region'");
        auto stmt = std::make_unique<RegionStmt>();
        stmt->name = expect(TokenType::IDENT, "expected region name").lexeme;
        stmt->body = parse_block();
        return stmt;
    }

    // ── unsafe ──────────────────────────────────────────────────────

    StmtPtr Parser::parse_unsafe() {
        expect(TokenType::UNSAFE, "expected 'unsafe'");
        auto stmt = std::make_unique<UnsafeStmt>();
        stmt->body = parse_block();
        return stmt;
    }

    // ── drop ────────────────────────────────────────────────────────

    StmtPtr Parser::parse_drop_stmt() {
        expect(TokenType::DROP, "expected 'drop'");
        expect(TokenType::LPAREN, "expected '('");
        auto stmt = std::make_unique<DropStmt>();
        stmt->expr = parse_expression();
        expect(TokenType::RPAREN, "expected ')'");
        return stmt;
    }

    // ── panic ───────────────────────────────────────────────────────

    StmtPtr Parser::parse_panic() {
        expect(TokenType::PANIC, "expected 'panic'");
        expect(TokenType::LPAREN, "expected '('");
        auto stmt    = std::make_unique<PanicStmt>();
        stmt->message = parse_expression();
        expect(TokenType::RPAREN, "expected ')'");
        return stmt;
    }

    // ── spawn ───────────────────────────────────────────────────────

    StmtPtr Parser::parse_spawn() {
        expect(TokenType::SPAWN, "expected 'spawn'");
        auto stmt = std::make_unique<SpawnStmt>();
        // spawn is typically used in: let handle = spawn expr
        // but can also appear as statement
        stmt->expr = parse_expression();
        return stmt;
    }

    // ── send ────────────────────────────────────────────────────────

    StmtPtr Parser::parse_send() {
        expect(TokenType::SEND, "expected 'send'");
        auto stmt    = std::make_unique<SendStmt>();
        stmt->channel = expect(TokenType::IDENT, "expected channel name").lexeme;
        expect(TokenType::SEND_ARROW, "expected '<-'");
        stmt->value  = parse_expression();
        return stmt;
    }

    // ── select ──────────────────────────────────────────────────────

    StmtPtr Parser::parse_select() {
        expect(TokenType::SELECT, "expected 'select'");
        expect(TokenType::LBRACE, "expected '{'");
        auto stmt = std::make_unique<SelectStmt>();

        while (!check(TokenType::RBRACE) && !is_at_end()) {
            SelectArm arm;

            if (match(TokenType::DEFAULT)) {
                arm.kind = SelectArm::Kind::Default;
                expect(TokenType::FAT_ARROW, "expected '=>'");
                arm.body = check(TokenType::LBRACE)
                ? static_cast<StmtPtr>(parse_block())
                : [&]{ auto e=std::make_unique<ExpressionStmt>(); e->expression=parse_expression(); return StmtPtr(std::move(e)); }();
            } else if (check(TokenType::RECV)) {
                advance();
                arm.kind    = SelectArm::Kind::Recv;
                arm.channel = expect(TokenType::IDENT, "expected channel name").lexeme;
                expect(TokenType::ARROW, "expected '->'");
                arm.bind_name = expect(TokenType::IDENT, "expected variable name").lexeme;
                expect(TokenType::FAT_ARROW, "expected '=>'");
                arm.body = check(TokenType::LBRACE)
                ? static_cast<StmtPtr>(parse_block())
                : [&]{ auto e=std::make_unique<ExpressionStmt>(); e->expression=parse_expression(); return StmtPtr(std::move(e)); }();
            } else if (check(TokenType::SEND)) {
                advance();
                arm.kind    = SelectArm::Kind::Send;
                arm.channel = expect(TokenType::IDENT, "expected channel name").lexeme;
                expect(TokenType::SEND_ARROW, "expected '<-'");
                arm.send_value = parse_expression();
                expect(TokenType::FAT_ARROW, "expected '=>'");
                arm.body = check(TokenType::LBRACE)
                ? static_cast<StmtPtr>(parse_block())
                : [&]{ auto e=std::make_unique<ExpressionStmt>(); e->expression=parse_expression(); return StmtPtr(std::move(e)); }();
            } else {
                throw error("expected 'recv', 'send' or 'default' in select");
            }

            stmt->arms.push_back(std::move(arm));
        }

        expect(TokenType::RBRACE, "expected '}'");
        return stmt;
    }

    // ================================================================
    // Expressions — Pratt parser
    // Binding power table (higher = tighter binding)
    // ================================================================

    int Parser::infix_binding_power(TokenType op) {
        switch (op) {
            case TokenType::OR_OR:      return 10;
            case TokenType::AND_AND:    return 20;
            case TokenType::PIPE:       return 30;
            case TokenType::CARET:      return 40;
            case TokenType::AMP:        return 50;
            case TokenType::EQEQ:
            case TokenType::BANG_EQ:    return 60;
            case TokenType::LT:
            case TokenType::GT:
            case TokenType::LE:
            case TokenType::GE:         return 70;
            case TokenType::LSHIFT:
            case TokenType::RSHIFT:     return 80;
            case TokenType::PLUS:
            case TokenType::MINUS:      return 90;
            case TokenType::STAR:
            case TokenType::SLASH:
            case TokenType::MOD:        return 100;
            case TokenType::AS:         return 110;  // cast
            case TokenType::DOT_DOT:
            case TokenType::DOT_DOT_EQ: return 5;   // range (lowest after logical)
            default:                    return -1;
        }
    }

    bool Parser::is_infix_op(TokenType t) {
        return infix_binding_power(t) >= 0;
    }

    bool Parser::is_assignment_op(TokenType t) {
        switch (t) {
            case TokenType::EQ:
            case TokenType::PLUS_EQ:  case TokenType::MINUS_EQ:
            case TokenType::STAR_EQ:  case TokenType::SLASH_EQ:  case TokenType::MOD_EQ:
            case TokenType::AMP_EQ:   case TokenType::PIPE_EQ:   case TokenType::CARET_EQ:
            case TokenType::LSHIFT_EQ: case TokenType::RSHIFT_EQ:
                return true;
            default: return false;
        }
    }

    ExprPtr Parser::parse_expression() {
        return parse_expr_bp(0);
    }

    ExprPtr Parser::parse_expr_bp(int min_bp) {
        auto left = parse_prefix();

        while (true) {
            // Postfix operators: () . [] ?
            left = parse_postfix(std::move(left));

            // Infix operators
            int bp = infix_binding_power(current().type);
            if (bp < 0 || bp <= min_bp) break;

            TokenType op_type = current().type;
            std::string op = advance().lexeme;

            // Cast: expr as Type
            if (op_type == TokenType::AS) {
                auto cast = std::make_unique<CastExpr>();
                cast->expr        = std::move(left);
                cast->target_type = parse_type();
                left = std::move(cast);
                continue;
            }

            // Range: expr .. expr   expr ..= expr
            if (op_type == TokenType::DOT_DOT || op_type == TokenType::DOT_DOT_EQ) {
                auto range = std::make_unique<RangeExpr>();
                range->start     = std::move(left);
                range->inclusive = (op_type == TokenType::DOT_DOT_EQ);
                range->end       = parse_expr_bp(bp);
                left = std::move(range);
                continue;
            }

            auto bin = std::make_unique<BinaryExpr>();
            bin->left  = std::move(left);
            bin->op    = op;
            bin->right = parse_expr_bp(bp);
            left = std::move(bin);
        }

        return left;
    }

    // ── Prefix expressions ──────────────────────────────────────────

    ExprPtr Parser::parse_prefix() {
        // Unary operators: - ! ~ * &
        if (check_any({TokenType::MINUS, TokenType::BANG, TokenType::TILDE,
            TokenType::STAR,  TokenType::AMP})) {
            std::string op = advance().lexeme;
            auto unary = std::make_unique<UnaryExpr>();
            unary->op      = op;
            unary->operand = parse_expr_bp(110); // high bp for unary
            return unary;
            }

            // move expr
            if (match(TokenType::MOVE)) {
                auto m = std::make_unique<MoveExpr>();
                m->expr = parse_expr_bp(110);
                return m;
            }

            // await expr
            if (match(TokenType::AWAIT)) {
                auto a = std::make_unique<AwaitExpr>();
                a->task = parse_expr_bp(110);
                return a;
            }

            // recv expr
            if (match(TokenType::RECV)) {
                auto r = std::make_unique<RecvExpr>();
                r->channel = parse_expr_bp(110);
                return r;
            }

            // spawn expr
            if (match(TokenType::SPAWN)) {
                auto s = std::make_unique<SpawnExpr>();
                s->expr = parse_expr_bp(110);
                return s;
            }

            return parse_primary();
    }

    // ── Postfix expressions ─────────────────────────────────────────

    ExprPtr Parser::parse_postfix(ExprPtr left) {
        while (true) {
            // Function call: expr(args)
            if (check(TokenType::LPAREN)) {
                advance();
                auto call = std::make_unique<CallExpr>();
                call->callee    = std::move(left);
                call->arguments = parse_argument_list();
                expect(TokenType::RPAREN, "expected ')'");
                left = std::move(call);
                continue;
            }

            // Member access, method call, tuple access: expr.field  expr.method()  expr.0
            if (check(TokenType::DOT)) {
                advance();

                // Tuple index: .0 .1 ...
                if (check(TokenType::INTEGER)) {
                    auto ta  = std::make_unique<TupleAccessExpr>();
                    ta->object = std::move(left);
                    ta->index  = std::stoi(advance().lexeme);
                    left = std::move(ta);
                    continue;
                }

                std::string field = expect(TokenType::IDENT, "expected field or method name").lexeme;

                // Method call: .method(args)
                if (check(TokenType::LPAREN)) {
                    advance();
                    auto mc = std::make_unique<MethodCallExpr>();
                    mc->object    = std::move(left);
                    mc->method    = field;
                    mc->arguments = parse_argument_list();
                    expect(TokenType::RPAREN, "expected ')'");
                    left = std::move(mc);
                } else {
                    // Field access
                    auto ma  = std::make_unique<MemberExpr>();
                    ma->object = std::move(left);
                    ma->field  = field;
                    left = std::move(ma);
                }
                continue;
            }

            // Index: expr[i]
            if (check(TokenType::LBRACKET)) {
                advance();
                auto idx   = std::make_unique<IndexExpr>();
                idx->object = std::move(left);
                idx->index  = parse_expression();
                expect(TokenType::RBRACKET, "expected ']'");
                left = std::move(idx);
                continue;
            }

            // Error propagation: expr?
            if (check(TokenType::QUESTION)) {
                advance();
                auto unary = std::make_unique<UnaryExpr>();
                unary->op      = "?";
                unary->operand = std::move(left);
                left = std::move(unary);
                continue;
            }

            break;
        }
        return left;
    }

    // ── Primary expressions ──────────────────────────────────────────

    ExprPtr Parser::parse_primary() {
        // Literals
        if (check(TokenType::INTEGER)) {
            auto lit = std::make_unique<LiteralExpr>();
            lit->value = advance().lexeme;
            lit->kind  = LiteralKind::Int;
            return lit;
        }
        if (check(TokenType::FLOAT)) {
            auto lit = std::make_unique<LiteralExpr>();
            lit->value = advance().lexeme;
            lit->kind  = LiteralKind::Float;
            return lit;
        }
        if (check(TokenType::STRING)) {
            auto lit = std::make_unique<LiteralExpr>();
            lit->value = advance().lexeme;
            lit->kind  = LiteralKind::String;
            return lit;
        }
        if (check(TokenType::BOOL_TRUE) || check(TokenType::BOOL_FALSE)) {
            auto lit = std::make_unique<LiteralExpr>();
            lit->value = advance().lexeme;
            lit->kind  = LiteralKind::Bool;
            return lit;
        }

        // Interpolated string
        if (check(TokenType::INTERP_STRING))
            return parse_interpolated_string();

        // None
        if (match(TokenType::NONE))
            return std::make_unique<NoneExpr>();

        // Ok(expr)
        if (match(TokenType::OK)) {
            expect(TokenType::LPAREN, "expected '(' after Ok");
            auto ok = std::make_unique<OkExpr>();
            ok->expr = parse_expression();
            expect(TokenType::RPAREN, "expected ')'");
            return ok;
        }

        // Err(expr)
        if (match(TokenType::ERR)) {
            expect(TokenType::LPAREN, "expected '(' after Err");
            auto err = std::make_unique<ErrExpr>();
            err->expr = parse_expression();
            expect(TokenType::RPAREN, "expected ')'");
            return err;
        }

        // Some(expr)
        if (match(TokenType::SOME)) {
            expect(TokenType::LPAREN, "expected '(' after Some");
            auto some = std::make_unique<SomeExpr>();
            some->expr = parse_expression();
            expect(TokenType::RPAREN, "expected ')'");
            return some;
        }

        // new T(args)  new [n]T
        if (check(TokenType::NEW))
            return parse_alloc();

        // drop(expr)
        if (match(TokenType::DROP)) {
            expect(TokenType::LPAREN, "expected '('");
            auto d = std::make_unique<DropExpr>();
            d->expr = parse_expression();
            expect(TokenType::RPAREN, "expected ')'");
            return d;
        }

        // channel()
        if (check(TokenType::CHANNEL)) {
            advance();
            expect(TokenType::LPAREN, "expected '('");
            expect(TokenType::RPAREN, "expected ')'");
            return std::make_unique<ChannelExpr>();
        }

        // lambda: fn(params) -> type { body }
        if (check(TokenType::FN))
            return parse_lambda();

        // Tuple or grouped expression: (a, b)  (expr)
        if (check(TokenType::LPAREN))
            return parse_tuple_or_grouped();

        // Identifier
        if (check(TokenType::IDENT)) {
            auto id = std::make_unique<IdentifierExpr>();
            id->name = advance().lexeme;
            return id;
        }

        throw error("expected expression, got '" + current().lexeme + "'");
    }

    // ── Alloc ───────────────────────────────────────────────────────

    ExprPtr Parser::parse_alloc() {
        expect(TokenType::NEW, "expected 'new'");
        auto alloc = std::make_unique<AllocExpr>();

        if (match(TokenType::LBRACKET)) {
            // new [n]T
            alloc->is_array  = true;
            alloc->array_size = parse_expression();
            expect(TokenType::RBRACKET, "expected ']'");
            alloc->alloc_type = parse_type();
        } else {
            alloc->alloc_type = parse_type();
            if (match(TokenType::LPAREN)) {
                alloc->arguments = parse_argument_list();
                expect(TokenType::RPAREN, "expected ')'");
            }
        }
        return alloc;
    }

    // ── Lambda ──────────────────────────────────────────────────────

    ExprPtr Parser::parse_lambda() {
        expect(TokenType::FN, "expected 'fn'");
        auto lambda = std::make_unique<LambdaExpr>();

        expect(TokenType::LPAREN, "expected '('");
        if (!check(TokenType::RPAREN)) {
            LambdaParam p;
            p.name = expect(TokenType::IDENT, "expected parameter name").lexeme;
            if (match(TokenType::REF)) p.is_ref = true;
            p.type = parse_type();
            lambda->parameters.push_back(std::move(p));
            while (match(TokenType::COMMA)) {
                LambdaParam p2;
                p2.name = expect(TokenType::IDENT, "expected parameter name").lexeme;
                if (match(TokenType::REF)) p2.is_ref = true;
                p2.type = parse_type();
                lambda->parameters.push_back(std::move(p2));
            }
        }
        expect(TokenType::RPAREN, "expected ')'");
        if (match(TokenType::ARROW)) lambda->return_type = parse_type();
        lambda->body = parse_block();
        return lambda;
    }

    // ── Tuple or grouped expression ──────────────────────────────────

    ExprPtr Parser::parse_tuple_or_grouped() {
        expect(TokenType::LPAREN, "expected '('");
        auto first = parse_expression();

        if (match(TokenType::RPAREN)) {
            // Just a grouped expression: (expr)
            return first;
        }

        // Tuple: (expr, ...)
        expect(TokenType::COMMA, "expected ',' or ')'");
        auto tuple = std::make_unique<TupleExpr>();
        tuple->elements.push_back(std::move(first));

        while (!check(TokenType::RPAREN) && !is_at_end()) {
            tuple->elements.push_back(parse_expression());
            if (!match(TokenType::COMMA)) break;
        }

        expect(TokenType::RPAREN, "expected ')'");
        return tuple;
    }

    // ── Interpolated string ──────────────────────────────────────────

    ExprPtr Parser::parse_interpolated_string() {
        std::string raw = expect(TokenType::INTERP_STRING, "expected interpolated string").lexeme;
        auto interp = std::make_unique<InterpolatedStringExpr>();

        // Split raw content into text and {expr} parts
        // The lexer stored the raw string including { } markers
        // We parse inline here
        size_t i = 0;
        while (i < raw.size()) {
            if (raw[i] == '{') {
                // Find matching }
                size_t start = i + 1;
                int depth = 1;
                i++;
                while (i < raw.size() && depth > 0) {
                    if (raw[i] == '{') depth++;
                    else if (raw[i] == '}') depth--;
                    i++;
                }
                std::string expr_src = raw.substr(start, i - start - 1);

                // Parse expression from the embedded source
                // (simplified: store as identifier/raw text for now)
                InterpPart part;
                part.is_expr  = true;
                part.raw_text = expr_src; // the embedded parser would go here
                interp->parts.push_back(std::move(part));
            } else {
                // Collect raw text until {
                size_t start = i;
                while (i < raw.size() && raw[i] != '{') i++;
                InterpPart part;
                part.is_expr  = false;
                part.raw_text = raw.substr(start, i - start);
                interp->parts.push_back(std::move(part));
            }
        }

        return interp;
    }

    // ── Argument list ────────────────────────────────────────────────

    std::vector<ExprPtr> Parser::parse_argument_list() {
        std::vector<ExprPtr> args;
        if (check(TokenType::RPAREN)) return args;
        args.push_back(parse_expression());
        while (match(TokenType::COMMA) && !check(TokenType::RPAREN))
            args.push_back(parse_expression());
        return args;
    }

} // namespace sysp::parser
