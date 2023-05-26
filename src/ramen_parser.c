#include "ramen_parser.h"

#include <stdlib.h>
#include <memory.h>

bool ramen_parser_next_stmt(RamenParser* parser, RamenStmt* stmt)
{
    if(parser == NULL) return false;
    if(stmt == NULL) return false;
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
    block->data[block->size++] = stmt;
}


// bool ramen_parser_next_stmt(RamenParser* parser, RamenStmt* stmt)
// {
//     RamenStmt* next = NULL;
//     if(!ramen_parser_next_stmt(parser, next))
//         return false;

//     switch (parser->tokens[parser->i].type) {
//         case RAMEN_TOKEN_IDENTIFIER: {
            
//         }
//     }

//     return true;
// }