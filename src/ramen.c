#include <stdio.h>
#include "common.h"
#include "ramen_lexer.h"

int main(int argc, char** argv)
{
    const char* program_name = shift(&argc, &argv);
    StringView command = sv(shift(&argc, &argv));

    if(sv_eq(command, sv("compile"))) {
        const char* file_path = shift(&argc, &argv);
        char source[32000] = {0};
        uint32_t file_size = 0;
        if(!load_file_data(file_path, source, &file_size)) {
            return 1;
        }
        RamenLexer lex = { .i = 0, .source = sv(source), };
        RamenToken token = {0};
        RamenTokenList tokens = ramen_token_list_alloc();
        while(ramen_lexer_next_token(&lex, &token) && token.type != RAMEN_TOKEN_UNKNOWN) {
            ramen_token_list_append(&tokens, token);
        }

        for(size_t i = 0; i < tokens.size; ++i) {
            printf("[%d] %s "SV_FMT"\n", tokens.data[i].line,
            ramen_token_type_as_cstr(tokens.data[i].type), SV_ARGV(tokens.data[i].literal));
        }

        ramen_token_list_free(tokens);
    } else {
        fprintf(stderr, "USAGE: %s <command> [--options] (arguments)\n", program_name);
        fprintf(stderr, "All subcommands: \n");
        fprintf(stderr, "    compile [-o] (source_files)\n");
    }

    return 0;
}