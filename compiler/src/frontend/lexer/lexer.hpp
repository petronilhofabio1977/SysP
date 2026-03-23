#pragma once

#include <vector>
#include <string>
#include "../../core/token.hpp"

// ================================================================
// SysP Lexer — Grammar v7.0 Final
//
// Handles:
//   -- line comments
//   (* block comments *)
//   identifiers + all keywords
//   integer literals: decimal, 0xHEX, 0bBINARY, with _ separators
//   float literals: 1.0, 3.14
//   string literals with escape sequences
//   interpolated strings: f"hello {expr}"
//   all operators including bitwise, arrows, ranges
// ================================================================

class Lexer {
public:
    explicit Lexer(const std::string& source);

    std::vector<Token> tokenize();

    // Returns true if any errors occurred during tokenization
    bool has_errors() const { return !errors_.empty(); }
    const std::vector<std::string>& errors() const { return errors_; }

private:
    // ── Character navigation ──────────────────────────────────────
    char advance();
    char peek(int offset = 0) const;
    bool is_at_end() const;
    bool match(char expected);

    // ── Skip helpers ─────────────────────────────────────────────
    void skip_whitespace_and_comments();
    bool skip_line_comment();     // --
    bool skip_block_comment();    // (* ... *)

    // ── Token producers ──────────────────────────────────────────
    Token make_token(TokenType type, const std::string& lexeme) const;
    Token make_error(const std::string& msg) const;

    Token scan_identifier();
    Token scan_number();
    Token scan_string();
    Token scan_interpolated_string();   // f"..."
    Token scan_operator();

    // ── Helpers ──────────────────────────────────────────────────
    static bool is_digit(char c)       { return c >= '0' && c <= '9'; }
    static bool is_hex_digit(char c);
    static bool is_alpha(char c);
    static bool is_alnum(char c)       { return is_alpha(c) || is_digit(c); }

private:
    std::string             source_;
    size_t                  pos_    = 0;
    int                     line_   = 1;
    int                     column_ = 1;
    std::vector<std::string> errors_;
};
