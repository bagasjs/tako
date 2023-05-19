#ifndef SV_H
#define SV_H

#include <stddef.h>
#include <stdbool.h>

typedef struct StringView {
    const char* data;
    size_t size;
} StringView;

#define SV_FMT "%.*s"
#define SV_ARGV(sv) (int)sv.size, sv.data
#define SV_NULL (StringView){0}

StringView sv(const char* data);
StringView sv_slice(StringView strv, size_t start, size_t end);

int sv_find_cstr(StringView strv, const char* sth, size_t index);
int sv_find(StringView strv, StringView sth, size_t index);

bool sv_contains(StringView strv, StringView sth);
bool sv_has_prefix(StringView strv, StringView prefix);
bool sv_has_suffix(StringView strv, StringView suffix);
bool sv_eq(StringView a, StringView b);
int sv_intval(StringView strv);
void sv_split(StringView strv, StringView sep, StringView* results, size_t* results_size);


#if defined(SV_IMPLEMENTATION)

size_t _strlen(const char* cstr) 
{
    size_t i = 0;
    while(cstr[++i] != '\0');
    return i;
}

StringView sv(const char* data)
{
    return (StringView) {
        .data = data,
        .size = _strlen(data),
    };
}

StringView sv_slice(StringView strv, size_t start, size_t end)
{
    if(end < start) {
        size_t swapper = end;
        end = start;
        start = swapper;
    }

    if(strv.size < start) 
        return SV_NULL;

    return (StringView) {
        .data = strv.data + start,
        .size = end - start,
    };
}

bool sv_eq(StringView a, StringView b)
{
    if(a.size < b.size)
        return false;
    for(size_t i = 0; i < b.size; ++i) {
        if(a.data[i] != b.data[i]) 
            return false;
    }
    return true;
}

bool sv_contains(StringView strv, StringView sth)
{
    if(strv.size < sth.size)
        return false;
    for(size_t i = 0; i < strv.size; ++i) {
        if(strv.data[i] == sth.data[0]) {
            StringView cmp = sv_slice(strv, i, i + sth.size);
            if(sv_eq(sth, cmp))
                return true;
        }
    }
    return false;
}

bool sv_has_prefix(StringView strv, StringView prefix)
{
    if(strv.size < prefix.size) {
        return false;
    }

    for(size_t i = 0; i < prefix.size; ++i) {
        if(strv.data[i] != prefix.data[i])
            return false;
    }
    return true;
}

bool sv_has_suffix(StringView strv, StringView suffix)
{
    if(strv.size < suffix.size)
        return false;
    for(int i = (int)suffix.size - 1; i >= 0; --i) {
        if(strv.data[strv.size - suffix.size + i] != suffix.data[i])
            return false;
    }
    return true;
}

int sv_find_cstr(StringView strv, const char* sth, size_t index)
{
    return sv_find(strv, sv(sth), index);
}

int sv_find(StringView strv, StringView sth, size_t index)
{
    if(strv.size < sth.size)
        return -1;

    size_t found_count = 0;
    for(size_t i = 0; i < strv.size; ++i) {
        if(strv.data[i] == sth.data[0]) {
            StringView cmp = sv_slice(strv, i, i + sth.size);
            if(sv_eq(sth, cmp)) {
                if(found_count == index)
                    return (int)i;
                ++found_count;
            }
        }
    }

    return -1;    
}

int sv_intval(StringView strv)
{
    int result = 0;
    for(size_t i = 0; i < strv.size && isdigit(strv.data[i]); ++i) {
        result = result * 10 + (int)strv.data[i] - '0';
    }
    return result;
}

#endif

#endif