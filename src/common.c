#include "common.h"
#include <assert.h>
#include <stdlib.h>

#define TAKO_IMPLEMENTATION
#include "tako.h"

#define SV_IMPLEMENTATION
#include "sv.h"

char* shift(int* argc, char*** argv)
{
    assert(*argc > 0);
    char* result = **argv;
    *argc -= 1;
    *argv += 1;
    return result;
}

bool load_file_data(const char* file_path, char* writable, uint32_t* file_size)
{
    FILE* f = fopen(file_path, "r");
    if(f == NULL) {
        return false;
    }

    fseek(f, 0, SEEK_END);
    *file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if(writable == NULL) {
        return false;
    }

    fread(writable, sizeof(char), *file_size, f);
    fclose(f);
    return true;
}