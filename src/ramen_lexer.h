#ifndef RAMEN_LEXER_H
#define RAMEN_LEXER_H

#include "common.h"
#include "sv.h"

typedef enum RamenTokenType {
    RAMEN_TOKEN_UNKNOWN = 0,
    RAMEN_TOKEN_IDENTIFIER,
    RAMEN_TOKEN_LITERAL_NUMBER,
    RAMEN_TOKEN_LITERAL_STRING,

    RAMEN_TOKEN_ADD,
    RAMEN_TOKEN_SUB,
    RAMEN_TOKEN_MUL,
    RAMEN_TOKEN_DIV,

    RAMEN_TOKEN_SEMICOLON,
    RAMEN_TOKEN_LPAREN,
    RAMEN_TOKEN_RPAREN,
    RAMEN_TOKEN_LCBRACE,
    RAMEN_TOKEN_RCBRACE,

    RAMEN_TOKEN_ASSIGN,
    RAMEN_TOKEN_EQ,
    RAMEN_TOKEN_NOT,
    RAMEN_TOKEN_NEQ,
    RAMEN_TOKEN_GT,
    RAMEN_TOKEN_GTE,
    RAMEN_TOKEN_LT,
    RAMEN_TOKEN_LTE,
} RamenTokenType;

const char* ramen_token_type_as_cstr(RamenTokenType type);

typedef struct RamenToken {
    RamenTokenType type;
    StringView literal;
    int line;
    const char* file_path;
} RamenToken;

typedef struct RamenLexer {
    uint64_t i;
    uint32_t current_line;
    StringView source;
    const char* file_path;
} RamenLexer;

bool ramen_lexer_next_token(RamenLexer* lexer, RamenToken* token);

#endif // RAMEN_LEXER_H