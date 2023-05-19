#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>

#define SV_IMPLEMENTATION
#include "sv.h"

#define TAKO_IMPLEMENTATION
#include "tako.h"

#define TAKOCC_MAXIMUM_TOKENS 1024
#define TAKOCC_SINGLE_LINE_MAXIMUM_EXPRESSIONS 20

static const char* keywords[] = {
    "var",
    "fn",
    "return",
};

#define keywords_count sizeof(keywords)/(sizeof(keywords[0]))

typedef enum TKCCTokenType {
    // Identifier tokens should be stated first
    TKCC_TOKEN_VAR,
    TKCC_TOKEN_FN,
    TKCC_TOKEN_RETURN,

    TKCC_TOKEN_ADD,
    TKCC_TOKEN_SUB,
    TKCC_TOKEN_MUL,
    TKCC_TOKEN_DIV,

    TKCC_TOKEN_SEMICOLON,
    TKCC_TOKEN_LPAREN,
    TKCC_TOKEN_RPAREN,
    TKCC_TOKEN_LCBRACE,
    TKCC_TOKEN_RCBRACE,

    TKCC_TOKEN_ASSIGN,
    TKCC_TOKEN_EQ,
    TKCC_TOKEN_NOT,
    TKCC_TOKEN_NEQ,
    TKCC_TOKEN_GT,
    TKCC_TOKEN_GTE,
    TKCC_TOKEN_LT,
    TKCC_TOKEN_LTE,

    TKCC_TOKEN_STRING,
    TKCC_TOKEN_INT,
    TKCC_TOKEN_IDENTIFIER,
} TKCCTokenType;

typedef struct {
    StringView literal;
    int type;
} TKCCToken;

typedef struct {
    StringView name;
    uint64_t addr;
} TKCCVar;

static struct {
    TKCCToken tokens[TAKOCC_MAXIMUM_TOKENS];
    uint64_t tokens_count;

    TKCCVar vars[TAKO_MAXIMUM_STACK_SIZE];
    uint64_t vars_count;
    uint64_t virtual_stack_count;

    TakoProgram program;
} TAKOCC = {0};

void takocc_push_inst(TakoInst inst) {
    TAKOCC.virtual_stack_count += tako_inst_type_stack_usage(inst.type);
    printf("RUNNING %s(%ld) (vsc:%zu)\n", tako_inst_type_as_cstr(inst.type), inst.operand.ival, TAKOCC.virtual_stack_count);
    tako_program_push_inst(&TAKOCC.program, inst);
}

uint64_t takocc_get_var_dynaddr(StringView name) {
    for(uint64_t i = 0; i < TAKOCC.vars_count; ++i) {
        if(sv_eq(TAKOCC.vars[i].name, name)) {
            uint64_t dynaddr = TAKOCC.virtual_stack_count - 1 - TAKOCC.vars[i].addr;
            printf("ADDR "SV_FMT" (vsc:%zu)(sta:%zu)(dyn:%zu)\n", SV_ARGV(TAKOCC.vars[i].name), 
            TAKOCC.virtual_stack_count, TAKOCC.vars[i].addr, dynaddr);
            return dynaddr;
        }
    }
    assert(0 && "Variable is not found");
}

void takocc_translate_tokens_into_program(void)
{
    size_t i = 0;
    while(i < TAKOCC.tokens_count) {
        switch(TAKOCC.tokens[i].type) {
            case TKCC_TOKEN_VAR: {
                StringView var_name = TAKOCC.tokens[i + 1].literal;
                assert(i + 2 < TAKOCC.tokens_count && 
                "ERROR: Expected identifier after \"var\" keyword but end of program found");
                assert(TAKOCC.tokens[i + 1].type == TKCC_TOKEN_IDENTIFIER && 
                "ERROR: Expected an identifier after \"var\" keyword");
                i += 2;
                takocc_push_inst(tako_inst_pushi(0));
                TAKOCC.vars[TAKOCC.vars_count].name = var_name;
                TAKOCC.vars[TAKOCC.vars_count].addr = TAKOCC.vars_count;
                TAKOCC.vars_count += 1;

                if(i < TAKOCC.tokens_count && TAKOCC.tokens[i].type == TKCC_TOKEN_ASSIGN) {
                    bool continue_expression = true;
                    TakoProgram operations = {0};
                    do {
                        i += 1;
                        assert(i < TAKOCC.tokens_count && "ERROR: Expection expression after assignment token");
                        if(TAKOCC.tokens[i].type == TKCC_TOKEN_IDENTIFIER) {
                            uint64_t addr = takocc_get_var_dynaddr(TAKOCC.tokens[i].literal);
                            takocc_push_inst(tako_inst_copy(addr));
                        } else if(TAKOCC.tokens[i].type == TKCC_TOKEN_INT) {
                            takocc_push_inst(tako_inst_pushi(sv_intval(TAKOCC.tokens[i].literal)));
                        } else {
                            assert(0 && "ERROR: Invalid assignment");
                        }
                        i += 1;
                        if(TAKOCC.tokens[i].type == TKCC_TOKEN_ADD)
                            tako_program_push_inst(&operations, tako_inst_addi());
                        else if(TAKOCC.tokens[i].type == TKCC_TOKEN_SUB)
                            tako_program_push_inst(&operations, tako_inst_subi());
                        else if(TAKOCC.tokens[i].type == TKCC_TOKEN_MUL)
                            tako_program_push_inst(&operations, tako_inst_muli());
                        else if(TAKOCC.tokens[i].type == TKCC_TOKEN_DIV)
                            tako_program_push_inst(&operations, tako_inst_divi());
                        else
                            continue_expression = false;
                    } while(continue_expression);
                    for(size_t i = 0; i < operations.insts_count; ++i) {
                        takocc_push_inst(operations.insts[i]);
                    }
                    takocc_push_inst(tako_inst_swap(takocc_get_var_dynaddr(var_name)));
                    takocc_push_inst(tako_inst_drop());
                }
            } break;
            default: {
                i += 1;
            } break;
        }
    }
}

void takocc_parse_tokens(const char* source, size_t source_len)
{
    size_t i = 0;
    while(i < source_len) {
        switch(source[i]) {
            case '+': {
                StringView literal = (StringView){ .data = &source[i], .size = 1};
                TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken) { 
                    .literal = literal, 
                    .type = TKCC_TOKEN_ADD };
            } break;
            case '-': {
                StringView literal = (StringView){ .data = &source[i], .size = 1};
                TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken) { 
                    .literal = literal, 
                    .type = TKCC_TOKEN_SUB };
            } break;
            case '*': {
                StringView literal = (StringView){ .data = &source[i], .size = 1};
                TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken) { 
                    .literal = literal, 
                    .type = TKCC_TOKEN_MUL };
            } break;
            case '/': {
                StringView literal = (StringView){ .data = &source[i], .size = 1};
                TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken) { 
                    .literal = literal, 
                    .type = TKCC_TOKEN_DIV };
            } break;
            case ';': {
                StringView literal = (StringView){ .data = &source[i], .size = 1};
                TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken) { 
                    .literal = literal, 
                    .type = TKCC_TOKEN_SEMICOLON };
            } break;
            case '{': {
                StringView literal = (StringView){ .data = &source[i], .size = 1};
                TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken) { 
                    .literal = literal, 
                    .type = TKCC_TOKEN_LCBRACE };
            } break;
            case '}': {
                StringView literal = (StringView){ .data = &source[i], .size = 1};
                TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken) { 
                    .literal = literal, 
                    .type = TKCC_TOKEN_RCBRACE };
            } break;
            case '(': {
                StringView literal = (StringView){ .data = &source[i], .size = 1};
                TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken) { 
                    .literal = literal, 
                    .type = TKCC_TOKEN_LPAREN };
            } break;
            case ')': {
                StringView literal = (StringView){ .data = &source[i], .size = 1};
                TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken) { 
                    .literal = literal, 
                    .type = TKCC_TOKEN_RPAREN };
            } break;

            case '=': {
                StringView literal = { .data = &source[i], .size = 1 };
                TKCCTokenType type = TKCC_TOKEN_ASSIGN;
                if(i + 1 < source_len && source[i + 1] == '=') {
                    type = TKCC_TOKEN_EQ;
                    literal.size = 2;
                }
                TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken){
                    .literal = literal,
                    .type = type};
            } break;
            case '>': {
                StringView literal = { .data = &source[i], .size = 1 };
                TKCCTokenType type = TKCC_TOKEN_GT;
                if(i + 1 < source_len && source[i + 1] == '=') {
                    type = TKCC_TOKEN_GTE;
                    literal.size = 2;
                }
                TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken){
                    .literal = literal,
                    .type = type};
            } break;
            case '<': {
                StringView literal = { .data = &source[i], .size = 1 };
                TKCCTokenType type = TKCC_TOKEN_LT;
                if(i + 1 < source_len && source[i + 1] == '=') {
                    type = TKCC_TOKEN_LTE;
                    literal.size = 2;
                }
                TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken){
                    .literal = literal,
                    .type = type};
            } break;
            case '!': {
                StringView literal = { .data = &source[i], .size = 1 };
                TKCCTokenType type = TKCC_TOKEN_NOT;
                if(i + 1 < source_len && source[i + 1] == '=') {
                    type = TKCC_TOKEN_NEQ;
                    literal.size = 2;
                }
                TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken){
                    .literal = literal,
                    .type = type};
            } break;

            // // Check string
            // case '"': {
            //     StringView literal = { .data = &source[i], .size = 1 };
            //     i += 1;
            //     while(source[i] != '"' && source[i - 1] != '\\') {
            //         literal.size += 1;
            //         i += 1;
            //     }
            //     literal.size += 1;
            //     TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken) {
            //         .literal = literal,
            //         .type = TKCC_TOKEN_STRING,
            //     };
            // } break;
            default : {
                // Check if number
                if(isdigit(source[i])) {
                    StringView literal = { .data = &source[i], .size = 1 };
                    while(i + 1 < source_len && isdigit(source[i + 1])) {
                        literal.size += 1;
                        i += 1;
                    }
                    TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken){
                        .literal = literal,
                        .type = TKCC_TOKEN_INT, };
                } else if(isalpha(source[i])) {
                    StringView literal = { .data = &source[i], .size = 1 };
                    while(i + 1 < source_len && isalnum(source[i + 1])) {
                        literal.size += 1;
                        i += 1;
                    }
                    TKCCTokenType type = TKCC_TOKEN_IDENTIFIER;
                    for(TKCCTokenType itype = TKCC_TOKEN_VAR; itype < keywords_count; ++itype) {
                        if(sv_eq(literal, sv(keywords[itype])))
                            type = itype;
                    }
                    TAKOCC.tokens[TAKOCC.tokens_count++] = (TKCCToken){
                        .literal = literal,
                        .type = type, };
                } else {

                }
            } break;
        }
        i += 1;
    }
}

#include <string.h>

const char* source = 
    "var x = 34\n"
    "var y = 35\n"
    "var a = x + y\n"
    // "x = 64\n"
    // "var b = x - y;\n"
    // "var c = x == y;\n"
    // "var d = x != y;\n"
    // "var e = x >= y;\n"
    // "var f = x <= y;\n"
    ;

int main(void)
{
    takocc_parse_tokens(source, strlen(source));
    takocc_translate_tokens_into_program();
    tako_save_program_to_file(&TAKOCC.program, "output.tako");
    return 0;
}