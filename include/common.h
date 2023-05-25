#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>

char* shift(int* argc, char*** argv);
uint64_t load_file_data(const char* file_path, char* writable);

#endif // COMMON_H