#include <stdio.h>
#include "ramen_lexer.h"

const char* source =
    "x=1+3\n"
    "y=2+4\n"
    "z=3+5\n"
    "a = x == y\n"
    "a = x == b\n"
    ;

int main(int argc, char** argv)
{
    RamenLexer lex = { .i = 0, .source = sv(source), };
    RamenToken token = {0};
    while(ramen_lexer_next_token(&lex, &token)) {
        printf("%s: "SV_FMT" at line %d\n", ramen_token_type_as_cstr(token.type), SV_ARGV(token.literal), token.line);
    }
    return 0;
}