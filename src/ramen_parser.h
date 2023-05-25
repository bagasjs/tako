#ifndef RAMEN_PARSER_H
#define RAMEN_PARSER_H

#include "common.h"

typedef struct RamenBlock RamenBlock;

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

struct RamenBlock {
    uint64_t size;
    uint64_t capacity;
    RamenStmt *items;
};

#endif // RAMEN_PARSER_H