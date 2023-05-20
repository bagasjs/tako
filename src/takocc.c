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

const char* takocc_token_to_string(TKCCTokenType type) {
    switch(type) {
        case TKCC_TOKEN_VAR: return "TKCC_TOKEN_VAR"; 
        case TKCC_TOKEN_FN: return "TKCC_TOKEN_FN"; 
        case TKCC_TOKEN_RETURN: return "TKCC_TOKEN_RETURN"; 
        case TKCC_TOKEN_ADD: return "TKCC_TOKEN_ADD"; 
        case TKCC_TOKEN_SUB: return "TKCC_TOKEN_SUB"; 
        case TKCC_TOKEN_MUL: return "TKCC_TOKEN_MUL"; 
        case TKCC_TOKEN_DIV: return "TKCC_TOKEN_DIV"; 
        case TKCC_TOKEN_SEMICOLON: return "TKCC_TOKEN_SEMICOLON"; 
        case TKCC_TOKEN_LPAREN: return "TKCC_TOKEN_LPAREN"; 
        case TKCC_TOKEN_RPAREN: return "TKCC_TOKEN_RPAREN"; 
        case TKCC_TOKEN_LCBRACE: return "TKCC_TOKEN_LCBRACE"; 
        case TKCC_TOKEN_RCBRACE: return "TKCC_TOKEN_RCBRACE"; 
        case TKCC_TOKEN_ASSIGN: return "TKCC_TOKEN_ASSIGN"; 
        case TKCC_TOKEN_EQ: return "TKCC_TOKEN_EQ"; 
        case TKCC_TOKEN_NOT: return "TKCC_TOKEN_NOT"; 
        case TKCC_TOKEN_NEQ: return "TKCC_TOKEN_NEQ"; 
        case TKCC_TOKEN_GT: return "TKCC_TOKEN_GT"; 
        case TKCC_TOKEN_GTE: return "TKCC_TOKEN_GTE"; 
        case TKCC_TOKEN_LT: return "TKCC_TOKEN_LT"; 
        case TKCC_TOKEN_LTE: return "TKCC_TOKEN_LTE"; 
        case TKCC_TOKEN_STRING: return "TKCC_TOKEN_STRING"; 
        case TKCC_TOKEN_INT: return "TKCC_TOKEN_INT"; 
        case TKCC_TOKEN_IDENTIFIER: return "TKCC_TOKEN_IDENTIFIER"; 
    }
    return NULL;
}

const char* takocc_token_to_string(TKCCTokenType type);

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
    // printf("RUNNING %s(%ld) (vsc:%zu)\n", tako_inst_type_as_cstr(inst.type), inst.operand.ival, TAKOCC.virtual_stack_count);
    tako_program_push_inst(&TAKOCC.program, inst);
}

uint64_t takocc_get_var_dynaddr(StringView name) {
    for(uint64_t i = 0; i < TAKOCC.vars_count; ++i) {
        if(sv_eq(TAKOCC.vars[i].name, name)) {
            uint64_t dynaddr = TAKOCC.virtual_stack_count - 1 - TAKOCC.vars[i].addr;
            // printf("ADDR "SV_FMT" (vsc:%zu)(sta:%zu)(dyn:%zu)\n", SV_ARGV(TAKOCC.vars[i].name), 
            // TAKOCC.virtual_stack_count, TAKOCC.vars[i].addr, dynaddr);
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
                        else if(TAKOCC.tokens[i].type == TKCC_TOKEN_EQ)
                            tako_program_push_inst(&operations, tako_inst_eqi());
                        else if(TAKOCC.tokens[i].type == TKCC_TOKEN_NEQ)
                            tako_program_push_inst(&operations, tako_inst_neqi());
                        else if(TAKOCC.tokens[i].type == TKCC_TOKEN_GT)
                            tako_program_push_inst(&operations, tako_inst_gti());
                        else if(TAKOCC.tokens[i].type == TKCC_TOKEN_GTE)
                            tako_program_push_inst(&operations, tako_inst_gtei());
                        else if(TAKOCC.tokens[i].type == TKCC_TOKEN_LT)
                            tako_program_push_inst(&operations, tako_inst_lti());
                        else if(TAKOCC.tokens[i].type == TKCC_TOKEN_LTE)
                            tako_program_push_inst(&operations, tako_inst_ltei());
                        else
                            continue_expression = false;
                    } while(continue_expression);
                    for(size_t j = 0; j < operations.insts_count; ++j) {
                        takocc_push_inst(operations.insts[j]);
                    }
                    takocc_push_inst(tako_inst_swap(takocc_get_var_dynaddr(var_name)));
                    takocc_push_inst(tako_inst_drop());
                }
            } break;
            case TKCC_TOKEN_IDENTIFIER: {
                StringView var_name = TAKOCC.tokens[i].literal;
                i += 1;
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
                // printf(SV_FMT"\n", SV_ARGV(TAKOCC.tokens[i].literal));
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
                    i += 1;
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
                    i += 1;
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
                    i += 1;
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
                    i += 1;
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

void takocc_dump_tokens()
{
    for(size_t i = 0; i < TAKOCC.tokens_count; ++i) {
        printf("[%zu] [%s] "SV_FMT"\n", i, 
        takocc_token_to_string(TAKOCC.tokens[i].type), SV_ARGV(TAKOCC.tokens[i].literal));
    }
}

#include <string.h>

size_t get_file_size(const char* file_path)
{
    FILE* f = fopen(file_path, "r");
    if(f == NULL) {
        fprintf(stderr, "Failed to open file %s\n", file_path);
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    fclose(f);
    return sz;
}

void load_file_data(const char* file_path, char* writable, size_t file_size)
{
    FILE* f = fopen(file_path, "r");
    if(f == NULL) {
        fprintf(stderr, "Failed to open file %s\n", file_path);
        exit(EXIT_FAILURE);
    }

    if(writable == NULL) {
        fprintf(stderr, "Invalid writable buffer %p\n", writable);
        fclose(f);
        exit(EXIT_FAILURE);
    }
    fread(writable, sizeof(char), file_size, f);
    fclose(f);
}

int main(int argc, char** argv)
{
    if(argc < 3) {
        fprintf(stderr, "Invalid amount of arguments passed to %s\n", argv[0]);
        fprintf(stderr, "USAGE: %s <source-file> <output-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* source_path = argv[2];
    const char* output_path = argv[1];

    size_t file_size = get_file_size(source_path);
    char* source = malloc(sizeof(char) * file_size);
    takocc_parse_tokens(source, strlen(source));
    takocc_translate_tokens_into_program();
    tako_save_program_to_file(&TAKOCC.program, output_path);
    free(source);
    return 0;
}