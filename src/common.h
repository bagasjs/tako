#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

char* shift(int* argc, char*** argv);
bool load_file_data(const char* file_path, char* writable, uint32_t* file_size);

#endif // COMMON_H