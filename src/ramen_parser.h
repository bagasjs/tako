#ifndef RAMEN_PARSER_H
#define RAMEN_PARSER_H

#include "common.h"
#include "ramen_lexer.h"

typedef enum RamenExprType {
    RAMEN_EXPR_LITERAL_STRING,
    RAMEN_EXPR_LITERAL_BOOL,
    RAMEN_EXPR_LITERAL_NUMBER,
    RAMEN_EXPR_FUNCALL,
    RAMEN_EXPR_BINARY_OP,
    RAMEN_EXPR_VAL_READ,
} RamenExprType;

typedef struct RamenExpr {
    RamenExprType type;
} RamenExpr;

typedef enum RamenStmtType {
    RAMEN_STMT_EXPR,
    RAMEN_STMT_IF,
    RAMEN_STMT_VAR_ASSIGN,
    RAMEN_STMT_VAR_DEF,
    RAMEN_STMT_WHILE,
} RamenStmtType;

typedef struct RamenStmt {
    RamenStmtType type;
} RamenStmt;

typedef struct RamenBlock {
    uint64_t size;
    uint64_t capacity;
    RamenStmt* data;
} RamenBlock;

RamenBlock ramen_block_alloc(void);
void ramen_block_free(RamenBlock block);
void ramen_block_append_stmt(RamenBlock* block, RamenStmt stmt);

typedef struct RamenParser {
    RamenTokenList tokens;
    uint32_t i;
} RamenParser;

bool ramen_parser_next_stmt(RamenParser* parser, RamenStmt* stmt);

#endif // RAMEN_PARSER_H