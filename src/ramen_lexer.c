#include "ramen_lexer.h"

#include <stdio.h>
#include "sv.h"

bool ramen_lexer_next_token(RamenLexer* lexer, RamenToken* token)
{
    if(token == NULL) return false;

    bool status = true;

    token->literal = SV_NULL;
    token->type = RAMEN_TOKEN_UNKNOWN;
    token->line = lexer->current_line + 1;

    switch (lexer->source.data[lexer->i]) {
        case '\n': {
            lexer->current_line += 1;
        } break;
        case '+': {
            token->literal = (StringView){ .data = &lexer->source.data[lexer->i], .size = 1 };
            token->type = RAMEN_TOKEN_ADD;
        } break;
        case '-': {
            token->literal = (StringView){ .data = &lexer->source.data[lexer->i], .size = 1 };
            token->type = RAMEN_TOKEN_SUB;
        } break;
        case '*': {
            token->literal = (StringView){ .data = &lexer->source.data[lexer->i], .size = 1 };
            token->type = RAMEN_TOKEN_MUL;
        } break;
        case '/': {
            token->literal = (StringView){ .data = &lexer->source.data[lexer->i], .size = 1 };
            token->type = RAMEN_TOKEN_DIV;
        } break;
        case ';': {
            token->literal = (StringView){ .data = &lexer->source.data[lexer->i], .size = 1 };
            token->type = RAMEN_TOKEN_SEMICOLON;
        } break;
        case '{': {
            token->literal = (StringView){ .data = &lexer->source.data[lexer->i], .size = 1 };
            token->type = RAMEN_TOKEN_LCBRACE;
        } break;
        case '}': {
            token->literal = (StringView){ .data = &lexer->source.data[lexer->i], .size = 1 };
            token->type = RAMEN_TOKEN_RCBRACE;
        } break;
        case '(': {
            token->literal = (StringView){ .data = &lexer->source.data[lexer->i], .size = 1 };
            token->type = RAMEN_TOKEN_LPAREN;
        } break;
        case ')': {
            token->literal = (StringView){ .data = &lexer->source.data[lexer->i], .size = 1 };
            token->type = RAMEN_TOKEN_RPAREN;
        } break;
        case '=': {
            token->literal = (StringView){ .data = &lexer->source.data[lexer->i], .size = 1 };
            token->type = RAMEN_TOKEN_ASSIGN;
            if(lexer->i + 1 < lexer->source.size && lexer->source.data[lexer->i + 1] == '=') {
                token->literal.size += 1;
                token->type = RAMEN_TOKEN_EQ;
                lexer->i += 1;
            }
        } break;
        case '!': {
            token->literal = (StringView){ .data = &lexer->source.data[lexer->i], .size = 1 };
            token->type = RAMEN_TOKEN_NOT;
            if(lexer->i + 1 < lexer->source.size && lexer->source.data[lexer->i + 1] == '=') {
                token->literal.size += 1;
                token->type = RAMEN_TOKEN_NEQ;
                lexer->i += 1;
            }
        } break;
        case '>': {
            token->literal = (StringView){ .data = &lexer->source.data[lexer->i], .size = 1 };
            token->type = RAMEN_TOKEN_GT;
            if(lexer->i + 1 < lexer->source.size && lexer->source.data[lexer->i + 1] == '=') {
                token->literal.size += 1;
                token->type = RAMEN_TOKEN_GTE;
                lexer->i += 1;
            }
        } break;
        case '<': {
            token->literal = (StringView){ .data = &lexer->source.data[lexer->i], .size = 1 };
            token->type = RAMEN_TOKEN_LT;
            if(lexer->i + 1 < lexer->source.size && lexer->source.data[lexer->i + 1] == '=') {
                token->literal.size += 2;
                token->type = RAMEN_TOKEN_LTE;
                lexer->i += 1;
            }
        } break;
        default: {
            if(isdigit(lexer->source.data[lexer->i])) {
                token->literal.size += 1;
                token->literal.data = &lexer->source.data[lexer->i];
                while(lexer->i + 1 < lexer->source.size && isdigit(lexer->source.data[lexer->i + 1])) {
                    token->literal.size += 1;
                    lexer->i += 1;
                }
                token->type = RAMEN_TOKEN_LITERAL_NUMBER;
            } else if(isalpha(lexer->source.data[lexer->i])) {
                token->literal.size += 1;
                token->literal.data = &lexer->source.data[lexer->i];
                while(lexer->i + 1 < lexer->source.size && isalnum(lexer->source.data[lexer->i + 1])) {
                    token->literal.size += 1;
                    lexer->i += 1;
                }
                token->type = RAMEN_TOKEN_IDENTIFIER;
            }
        } break;
    }

    if(lexer->i + 1 < lexer->source.size) {
        lexer->i += 1;
        return status;
    } else {
        return false;
    }

}

const char* ramen_token_type_as_cstr(RamenTokenType type)
{
    switch(type) {
        case RAMEN_TOKEN_IDENTIFIER: return "RAMEN_TOKEN_IDENTIFIER";
        case RAMEN_TOKEN_LITERAL_NUMBER: return "RAMEN_TOKEN_LITERAL_NUMBER";
        case RAMEN_TOKEN_LITERAL_STRING: return "RAMEN_TOKEN_LITERAL_STRING";

        case RAMEN_TOKEN_ADD: return "RAMEN_TOKEN_ADD";
        case RAMEN_TOKEN_SUB: return "RAMEN_TOKEN_SUB";
        case RAMEN_TOKEN_MUL: return "RAMEN_TOKEN_MUL";
        case RAMEN_TOKEN_DIV: return "RAMEN_TOKEN_DIV";

        case RAMEN_TOKEN_SEMICOLON: return "RAMEN_TOKEN_SEMICOLON";
        case RAMEN_TOKEN_LPAREN: return "RAMEN_TOKEN_LPAREN";
        case RAMEN_TOKEN_RPAREN: return "RAMEN_TOKEN_RPAREN";
        case RAMEN_TOKEN_LCBRACE: return "RAMEN_TOKEN_LCBRACE";
        case RAMEN_TOKEN_RCBRACE: return "RAMEN_TOKEN_RCBRACE";

        case RAMEN_TOKEN_ASSIGN: return "RAMEN_TOKEN_ASSIGN";
        case RAMEN_TOKEN_EQ: return "RAMEN_TOKEN_EQ";
        case RAMEN_TOKEN_NOT: return "RAMEN_TOKEN_NOT";
        case RAMEN_TOKEN_NEQ: return "RAMEN_TOKEN_NEQ";
        case RAMEN_TOKEN_GT: return "RAMEN_TOKEN_GT";
        case RAMEN_TOKEN_GTE: return "RAMEN_TOKEN_GTE";
        case RAMEN_TOKEN_LT: return "RAMEN_TOKEN_LT";
        case RAMEN_TOKEN_LTE: return "RAMEN_TOKEN_LTE";
    }
    return "RAMEN_TOKEN_UNKNOWN";
}

