#include <stdio.h>
#include "tako.h"

int main(int argc, char** argv)
{
    TakoRuntime rt = {0};
    if(argc < 2) 
        printf("USAGE: %s <bytecode-path>", argv[0]);
    tako_load_program_from_file(&rt.program, argv[1]);
    tako_runtime_run(&rt, false);
    tako_stack_dump(&rt.stack);
    return 0;
}