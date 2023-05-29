#include "ramen_parser.h"

#include <stdlib.h>
#include <memory.h>

bool ramen_parser_next_stmt(RamenParser* parser, RamenStmt* stmt)
{
    if(parser == NULL) return false;
    if(stmt == NULL) return false;

    RamenToken ctoken = parser->tokens.data[parser->i];

    switch (ctoken.type) {
        case RAMEN_TOKEN_UNKNOWN: {
            return false;
        } break;
        case RAMEN_TOKEN_IDENTIFIER: {
            
        } break;
        case RAMEN_TOKEN_LITERAL_NUMBER: {

        } break;
        case RAMEN_TOKEN_LITERAL_STRING: {

        } break;
        case RAMEN_TOKEN_ADD: {

        } break;
        case RAMEN_TOKEN_SUB: {

        } break;
        case RAMEN_TOKEN_MUL: {

        } break;
        case RAMEN_TOKEN_DIV: {

        } break;
        case RAMEN_TOKEN_SEMICOLON: {

        } break;
        case RAMEN_TOKEN_LPAREN: {

        } break;
        case RAMEN_TOKEN_RPAREN: {

        } break;
        case RAMEN_TOKEN_LCBRACE: {

        } break;
        case RAMEN_TOKEN_RCBRACE: {

        } break;
        case RAMEN_TOKEN_ASSIGN: {

        } break;
        case RAMEN_TOKEN_EQ: {

        } break;
        case RAMEN_TOKEN_NOT: {

        } break;
        case RAMEN_TOKEN_NEQ: {

        } break;
        case RAMEN_TOKEN_GT: {

        } break;
        case RAMEN_TOKEN_GTE: {

        } break;
        case RAMEN_TOKEN_LT: {

        } break;
        case RAMEN_TOKEN_LTE: {

        } break;
        case RAMEN_TOKEN_WHITESPACE: {

        } break;
    }

    return true;
}

RamenBlock ramen_block_alloc(void)
{
    RamenStmt* stmts = malloc(sizeof(RamenStmt) * 4);
    return (RamenBlock) {
        .size = 0,
        .capacity = 4,
        .data = stmts,
    };
}

void ramen_block_free(RamenBlock block)
{
    free(block.data);
}

#include <stdio.h>

void ramen_block_append_stmt(RamenBlock* block, RamenStmt stmt)
{
    if(block == NULL) return;
    if(block->size + 1 > block->capacity) {
        RamenStmt* old_data = block->data;
        size_t old_capacity = block->capacity;
        block->capacity = old_capacity * 2;
        block->data = malloc(sizeof(RamenStmt) * block->capacity);
        memcpy(block->data, old_data, sizeof(RamenStmt) * old_capacity);
        free(old_data);
    }
    printf("Test\n");
    block->data[block->size++] = stmt;
}
