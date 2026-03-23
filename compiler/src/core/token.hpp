#pragma once
#include <string>

// ================================================================
// SysP Token Types — Grammar v7.0 Final
// ================================================================

enum class TokenType {

    // ── End of file ──────────────────────────────────────────────
    END,

    // ── Literals ─────────────────────────────────────────────────
    IDENT,
    INTEGER,
    FLOAT,
    STRING,
    INTERP_STRING,      // f"..." interpolated string
    BOOL_TRUE,          // true
    BOOL_FALSE,         // false

    // ── Keywords ─────────────────────────────────────────────────
    // declarations
    MODULE,
    FN,
    STRUCT,
    ENUM,
    TRAIT,
    IMPL,
    TYPE,
    CONST,
    PUB,
    SELF,
    WHERE,

    // variables
    LET,
    NEW,
    MOVE,
    DROP,

    // control flow
    IF,
    ELSE,
    WHILE,
    FOR,
    IN,
    LOOP,
    BREAK,
    CONTINUE,
    RETURN,
    MATCH,

    // memory
    REGION,
    UNSAFE,
    REF,
    AS,

    // error
    PANIC,

    // concurrency
    SPAWN,
    SEND,
    RECV,
    AWAIT,
    CHANNEL,
    SELECT,
    DEFAULT,

    // built-in types (treated as keywords)
    KW_I8,  KW_I16,  KW_I32,  KW_I64,
    KW_U8,  KW_U16,  KW_U32,  KW_U64,
    KW_F32, KW_F64,
    KW_BOOL,
    KW_STRING,

    // built-in generic types
    KW_RESULT,
    KW_OPTION,
    KW_TASK,
    KW_CHANNEL_TYPE,

    // built-in enum constructors
    OK,
    ERR,
    SOME,
    NONE,

    // ── Arithmetic operators ──────────────────────────────────────
    PLUS,           // +
    MINUS,          // -
    STAR,           // *
    SLASH,          // /
    MOD,            // %

    // ── Arithmetic assignment ─────────────────────────────────────
    PLUS_EQ,        // +=
    MINUS_EQ,       // -=
    STAR_EQ,        // *=
    SLASH_EQ,       // /=
    MOD_EQ,         // %=

    // ── Bitwise operators ─────────────────────────────────────────
    AMP,            // &
    PIPE,           // |
    CARET,          // ^
    TILDE,          // ~
    LSHIFT,         // <<
    RSHIFT,         // >>

    // ── Bitwise assignment ────────────────────────────────────────
    AMP_EQ,         // &=
    PIPE_EQ,        // |=
    CARET_EQ,       // ^=
    LSHIFT_EQ,      // <<=
    RSHIFT_EQ,      // >>=

    // ── Comparison ───────────────────────────────────────────────
    EQ,             // =
    EQEQ,           // ==
    BANG_EQ,        // !=

    LT,             // <
    GT,             // >
    LE,             // <=
    GE,             // >=

    // ── Logical ──────────────────────────────────────────────────
    AND_AND,        // &&
    OR_OR,          // ||
    BANG,           // !

    // ── Arrows and special ───────────────────────────────────────
    ARROW,          // ->   (return type, method call)
    FAT_ARROW,      // =>   (match arm)
    SEND_ARROW,     // <-   (channel send: send ch <- val)
    RECV_ARROW,     // ->   (channel recv in select: recv ch -> v)
    QUESTION,       // ?    (error propagation)
    AT,             // @    (pattern binding, reserved)

    // ── Range ────────────────────────────────────────────────────
    DOT_DOT,        // ..   (exclusive range)
    DOT_DOT_EQ,     // ..=  (inclusive range)

    // ── Punctuation ──────────────────────────────────────────────
    DOT,            // .
    COMMA,          // ,
    COLON,          // :
    COLON_COLON,    // ::   (reserved for future path syntax)
    SEMICOLON,      // ;    (reserved, not used in SysP)
    UNDERSCORE,     // _    (wildcard pattern)
    HASH,           // #    (reserved)

    // ── Delimiters ───────────────────────────────────────────────
    LPAREN,         // (
        RPAREN,         // )
        LBRACE,         // {
        RBRACE,         // }
        LBRACKET,       // [
        RBRACKET,       // ]

};

// ================================================================
// Token
// ================================================================

struct Token {
    TokenType   type;
    std::string lexeme;
    int         line;
    int         column;
};
