#include "lexer.hpp"
#include <cctype>
#include <unordered_map>
#include <stdexcept>

// ================================================================
// Keyword table — Grammar v7.0 Final
// ================================================================

static const std::unordered_map<std::string, TokenType> KEYWORDS = {
    // declarations
    { "module",   TokenType::MODULE   },
    { "fn",       TokenType::FN       },
    { "struct",   TokenType::STRUCT   },
    { "enum",     TokenType::ENUM     },
    { "trait",    TokenType::TRAIT    },
    { "impl",     TokenType::IMPL     },
    { "type",     TokenType::TYPE     },
    { "const",    TokenType::CONST    },
    { "pub",      TokenType::PUB      },
    { "self",     TokenType::SELF     },
    { "where",    TokenType::WHERE    },
    // variables
    { "let",      TokenType::LET      },
    { "new",      TokenType::NEW      },
    { "move",     TokenType::MOVE     },
    { "drop",     TokenType::DROP     },
    // control flow
    { "if",       TokenType::IF       },
    { "else",     TokenType::ELSE     },
    { "while",    TokenType::WHILE    },
    { "for",      TokenType::FOR      },
    { "in",       TokenType::IN       },
    { "loop",     TokenType::LOOP     },
    { "break",    TokenType::BREAK    },
    { "continue", TokenType::CONTINUE },
    { "return",   TokenType::RETURN   },
    { "match",    TokenType::MATCH    },
    // memory
    { "region",   TokenType::REGION   },
    { "unsafe",   TokenType::UNSAFE   },
    { "ref",      TokenType::REF      },
    { "as",       TokenType::AS       },
    // error
    { "panic",    TokenType::PANIC    },
    // concurrency
    { "spawn",    TokenType::SPAWN    },
    { "send",     TokenType::SEND     },
    { "recv",     TokenType::RECV     },
    { "await",    TokenType::AWAIT    },
    { "channel",  TokenType::CHANNEL  },
    { "select",   TokenType::SELECT   },
    { "default",  TokenType::DEFAULT  },
    // booleans
    { "true",     TokenType::BOOL_TRUE  },
    { "false",    TokenType::BOOL_FALSE },
    // primitive types
    { "i8",       TokenType::KW_I8     },
    { "i16",      TokenType::KW_I16    },
    { "i32",      TokenType::KW_I32    },
    { "i64",      TokenType::KW_I64    },
    { "u8",       TokenType::KW_U8     },
    { "u16",      TokenType::KW_U16    },
    { "u32",      TokenType::KW_U32    },
    { "u64",      TokenType::KW_U64    },
    { "f32",      TokenType::KW_F32    },
    { "f64",      TokenType::KW_F64    },
    { "bool",     TokenType::KW_BOOL   },
    { "string",   TokenType::KW_STRING },
    // generic types
    { "Result",   TokenType::KW_RESULT       },
    { "Option",   TokenType::KW_OPTION       },
    { "Task",     TokenType::KW_TASK         },
    { "Channel",  TokenType::KW_CHANNEL_TYPE },
    // enum constructors
    { "Ok",       TokenType::OK   },
    { "Err",      TokenType::ERR  },
    { "Some",     TokenType::SOME },
    { "None",     TokenType::NONE },
};

// ================================================================
// Lexer implementation
// ================================================================

Lexer::Lexer(const std::string& src) : source_(src) {}

// ── Character navigation ──────────────────────────────────────────

char Lexer::advance() {
    char c = source_[pos_++];
    if (c == '\n') { line_++; column_ = 1; }
    else           { column_++; }
    return c;
}

char Lexer::peek(int offset) const {
    size_t idx = pos_ + static_cast<size_t>(offset);
    if (idx >= source_.size()) return '\0';
    return source_[idx];
}

bool Lexer::is_at_end() const {
    return pos_ >= source_.size();
}

bool Lexer::match(char expected) {
    if (is_at_end() || source_[pos_] != expected) return false;
    advance();
    return true;
}

// ── Skip helpers ──────────────────────────────────────────────────

bool Lexer::skip_line_comment() {
    // Line comment starts with --
    if (peek(0) == '-' && peek(1) == '-') {
        while (!is_at_end() && peek() != '\n')
            advance();
        return true;
    }
    return false;
}

bool Lexer::skip_block_comment() {
    // Block comment: (* ... *)  (can be nested)
    if (peek(0) == '(' && peek(1) == '*') {
        advance(); advance(); // consume (*
        int depth = 1;
        while (!is_at_end() && depth > 0) {
            if (peek(0) == '(' && peek(1) == '*') {
                advance(); advance();
                depth++;
            } else if (peek(0) == '*' && peek(1) == ')') {
                advance(); advance();
                depth--;
            } else {
                advance();
            }
        }
        if (depth != 0)
            errors_.push_back("Unterminated block comment at line " + std::to_string(line_));
        return true;
    }
    return false;
}

void Lexer::skip_whitespace_and_comments() {
    while (!is_at_end()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else if (skip_line_comment()) {
            // consumed
        } else if (skip_block_comment()) {
            // consumed
        } else {
            break;
        }
    }
}

// ── Token helpers ─────────────────────────────────────────────────

Token Lexer::make_token(TokenType type, const std::string& lexeme) const {
    return Token{ type, lexeme, line_, column_ };
}

// ── Identifier and keyword scanning ──────────────────────────────

bool Lexer::is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::is_hex_digit(char c) {
    return is_digit(c)
    || (c >= 'a' && c <= 'f')
    || (c >= 'A' && c <= 'F');
}

Token Lexer::scan_identifier() {
    size_t start = pos_;
    int    start_col = column_;
    while (!is_at_end() && is_alnum(peek()))
        advance();

    std::string text = source_.substr(start, pos_ - start);

    auto it = KEYWORDS.find(text);
    TokenType type = (it != KEYWORDS.end()) ? it->second : TokenType::IDENT;

    return Token{ type, text, line_, start_col };
}

// ── Number scanning ───────────────────────────────────────────────

Token Lexer::scan_number() {
    size_t start     = pos_;
    int    start_col = column_;
    bool   is_float  = false;

    // Hex: 0x...
    if (peek(0) == '0' && (peek(1) == 'x' || peek(1) == 'X')) {
        advance(); advance(); // 0x
        while (!is_at_end() && (is_hex_digit(peek()) || peek() == '_'))
            advance();
        return Token{ TokenType::INTEGER, source_.substr(start, pos_ - start), line_, start_col };
    }

    // Binary: 0b...
    if (peek(0) == '0' && (peek(1) == 'b' || peek(1) == 'B')) {
        advance(); advance(); // 0b
        while (!is_at_end() && (peek() == '0' || peek() == '1' || peek() == '_'))
            advance();
        return Token{ TokenType::INTEGER, source_.substr(start, pos_ - start), line_, start_col };
    }

    // Decimal (possibly float)
    while (!is_at_end() && (is_digit(peek()) || peek() == '_'))
        advance();

    // Float part
    if (!is_at_end() && peek(0) == '.' && is_digit(peek(1))) {
        is_float = true;
        advance(); // .
        while (!is_at_end() && (is_digit(peek()) || peek() == '_'))
            advance();
    }

    TokenType type = is_float ? TokenType::FLOAT : TokenType::INTEGER;
    return Token{ type, source_.substr(start, pos_ - start), line_, start_col };
}

// ── String scanning ───────────────────────────────────────────────

Token Lexer::scan_string() {
    int start_col = column_;
    advance(); // opening "

    std::string value;
    while (!is_at_end() && peek() != '"') {
        char c = peek();
        if (c == '\\') {
            advance(); // backslash
            char esc = advance();
            switch (esc) {
                case 'n':  value += '\n'; break;
                case 't':  value += '\t'; break;
                case 'r':  value += '\r'; break;
                case '"':  value += '"';  break;
                case '\\': value += '\\'; break;
                case '0':  value += '\0'; break;
                case '{':  value += '{';  break;
                default:
                    errors_.push_back("Unknown escape '" + std::string(1, esc) + "' at line " + std::to_string(line_));
                    value += esc;
            }
        } else {
            value += advance();
        }
    }

    if (is_at_end()) {
        errors_.push_back("Unterminated string at line " + std::to_string(line_));
        return Token{ TokenType::STRING, value, line_, start_col };
    }

    advance(); // closing "
    return Token{ TokenType::STRING, value, line_, start_col };
}

// ── Interpolated string scanning ─────────────────────────────────
// f"text {expr} text"
// The lexer stores the raw content including { } markers.
// The parser will split it into parts.

Token Lexer::scan_interpolated_string() {
    int start_col = column_;
    advance(); // f
    advance(); // opening "

    std::string raw;
    int depth = 0;

    while (!is_at_end()) {
        char c = peek();

        if (c == '"' && depth == 0) {
            advance(); // closing "
            break;
        }

        if (c == '{') {
            depth++;
            raw += advance();
            continue;
        }

        if (c == '}') {
            if (depth > 0) depth--;
            raw += advance();
            continue;
        }

        if (c == '\\' && depth == 0) {
            raw += advance(); // backslash
            if (!is_at_end()) {
                raw += advance(); // escaped char
            }
            continue;
        }

        raw += advance();
    }

    return Token{ TokenType::INTERP_STRING, raw, line_, start_col };
}

// ── Operator scanning ─────────────────────────────────────────────

Token Lexer::scan_operator() {
    int start_col = column_;
    char c = advance();

    auto tok = [&](TokenType t, const std::string& lex) -> Token {
        return Token{ t, lex, line_, start_col };
    };

    switch (c) {

        // ── Arithmetic ──────────────────────────────────────────────
        case '+':
            if (match('=')) return tok(TokenType::PLUS_EQ, "+=");
            return tok(TokenType::PLUS, "+");

        case '-':
            if (peek() == '>') { advance(); return tok(TokenType::ARROW, "->"); }
            if (match('='))    return tok(TokenType::MINUS_EQ, "-=");
            return tok(TokenType::MINUS, "-");

        case '*':
            if (match('=')) return tok(TokenType::STAR_EQ, "*=");
            return tok(TokenType::STAR, "*");

        case '/':
            if (match('=')) return tok(TokenType::SLASH_EQ, "/=");
            return tok(TokenType::SLASH, "/");

        case '%':
            if (match('=')) return tok(TokenType::MOD_EQ, "%=");
            return tok(TokenType::MOD, "%");

        // ── Bitwise ─────────────────────────────────────────────────
        case '&':
            if (peek() == '&') { advance(); return tok(TokenType::AND_AND, "&&"); }
            if (match('='))    return tok(TokenType::AMP_EQ, "&=");
            return tok(TokenType::AMP, "&");

        case '|':
            if (peek() == '|') { advance(); return tok(TokenType::OR_OR, "||"); }
            if (match('='))    return tok(TokenType::PIPE_EQ, "|=");
            return tok(TokenType::PIPE, "|");

        case '^':
            if (match('=')) return tok(TokenType::CARET_EQ, "^=");
            return tok(TokenType::CARET, "^");

        case '~':
            return tok(TokenType::TILDE, "~");

            // ── Comparison and assignment ────────────────────────────────
        case '=':
            if (peek() == '>') { advance(); return tok(TokenType::FAT_ARROW, "=>"); }
            if (match('='))    return tok(TokenType::EQEQ, "==");
            return tok(TokenType::EQ, "=");

        case '!':
            if (match('=')) return tok(TokenType::BANG_EQ, "!=");
            return tok(TokenType::BANG, "!");

        case '<':
            if (peek() == '<') {
                advance();
                if (match('=')) return tok(TokenType::LSHIFT_EQ, "<<=");
                return tok(TokenType::LSHIFT, "<<");
            }
            if (peek() == '-') { advance(); return tok(TokenType::SEND_ARROW, "<-"); }
            if (match('='))    return tok(TokenType::LE, "<=");
            return tok(TokenType::LT, "<");

        case '>':
            if (peek() == '>') {
                advance();
                if (match('=')) return tok(TokenType::RSHIFT_EQ, ">>=");
                return tok(TokenType::RSHIFT, ">>");
            }
            if (match('=')) return tok(TokenType::GE, ">=");
            return tok(TokenType::GT, ">");

        // ── Range and dot ────────────────────────────────────────────
        case '.':
            if (peek() == '.') {
                advance();
                if (match('=')) return tok(TokenType::DOT_DOT_EQ, "..=");
                return tok(TokenType::DOT_DOT, "..");
            }
            return tok(TokenType::DOT, ".");

            // ── Error propagation ────────────────────────────────────────
        case '?':
            return tok(TokenType::QUESTION, "?");

            // ── Punctuation ──────────────────────────────────────────────
        case ',': return tok(TokenType::COMMA,    ",");
        case ':':
            if (match(':')) return tok(TokenType::COLON_COLON, "::");
            return tok(TokenType::COLON, ":");
        case ';': return tok(TokenType::SEMICOLON, ";");
        case '_': return tok(TokenType::UNDERSCORE, "_");
        case '#': return tok(TokenType::HASH, "#");
        case '@': return tok(TokenType::AT, "@");

        // ── Delimiters ───────────────────────────────────────────────
        case '(': return tok(TokenType::LPAREN,   "(");
        case ')': return tok(TokenType::RPAREN,   ")");
        case '{': return tok(TokenType::LBRACE,   "{");
        case '}': return tok(TokenType::RBRACE,   "}");
        case '[': return tok(TokenType::LBRACKET, "[");
        case ']': return tok(TokenType::RBRACKET, "]");

        default:
            errors_.push_back(
                "Unexpected character '" + std::string(1, c) +
                "' at line " + std::to_string(line_) +
                " col " + std::to_string(start_col)
            );
            return tok(TokenType::END, "");
    }
}

// ── Main tokenize loop ────────────────────────────────────────────

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (true) {
        skip_whitespace_and_comments();

        if (is_at_end()) break;

        char c = peek();

        // Interpolated string: f"..."
        if (c == 'f' && peek(1) == '"') {
            tokens.push_back(scan_interpolated_string());
            continue;
        }

        // Identifier or keyword
        if (is_alpha(c)) {
            tokens.push_back(scan_identifier());
            continue;
        }

        // Number
        if (is_digit(c)) {
            tokens.push_back(scan_number());
            continue;
        }

        // String
        if (c == '"') {
            tokens.push_back(scan_string());
            continue;
        }

        // Operators and punctuation
        tokens.push_back(scan_operator());
    }

    tokens.push_back(Token{ TokenType::END, "", line_, column_ });
    return tokens;
}
