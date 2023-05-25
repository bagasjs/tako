#include "common.h"
#include <assert.h>

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

uint64_t load_file_data(const char* file_path, char* writable)
{
    
}