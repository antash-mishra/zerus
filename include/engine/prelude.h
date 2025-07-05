//
// Created by akash on 28/6/25.
//

#ifndef PRELUDE_H
#define PRELUDE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

typedef struct
{
    void* (*malloc)(ptrdiff_t, void* ctx);
    void (*free)(void*, void* ctx);
    void* ctx;
} allocator;

typedef enum
{
    ALLOC_FAILED
} prelude_err;

typedef struct
{
    const char* chars;
    size_t      len;
} string_t;

typedef struct string_array_t
{
    size_t   len;
    size_t   cap;
    string_t data[];
} string_array_t;


#define MAKE_STR(literal) { .chars = (literal), .len = sizeof(literal) - 1 }


size_t find_length_of_c_string(const char str[])
{
    size_t len = 0;
    while (str[len] != '\0')
    {
        len++;
    }

    return len;
}

string_t make_from_c_string(const char* str)
{
    return (string_t) { .chars = str, .len = find_length_of_c_string(str) };
}

bool string_equal(string_t lhs, string_t rhs)
{
    if (lhs.len != rhs.len)
    {
        return false;
    }

    for (size_t i = 0; i < lhs.len; i++)
    {
        if (lhs.chars[i] != rhs.chars[i])
        {
            return false;
        }
    }

    return true;
}

string_array_t* make_string_array(allocator* alloc, size_t cap)
{
    size_t size = sizeof(string_array_t) + cap * sizeof(string_t);

    string_array_t* arr = alloc->malloc(size, alloc->ctx);
    if (!arr)
    {
        return NULL;
    }

    arr->len = 0;
    arr->cap = cap;

    return arr;
}

bool string_array_push(allocator* alloc, string_array_t* arr, string_t str)
{
    if (!arr)
    {
        return false;
    }

    // Check if we need to grow the array
    if (arr->len >= arr->cap)
    {
        size_t new_cap  = arr->cap == 0 ? 8 : arr->cap * 2;
        size_t new_size = sizeof(string_array_t) + new_cap * sizeof(string_t);
        string_array_t* new_arr = alloc->malloc(new_size, alloc->ctx);
        if (!new_arr)
        {
            return false;
        }

        new_arr->len = arr->len;
        new_arr->cap = new_cap;

        // copy existing data
        for (size_t i = 0; i < arr->len; i++)
        {
            new_arr->data[i] = arr->data[i];
        }

        // Free old array
        alloc->free(arr, alloc->ctx);
        arr = new_arr;
    }

    // Add the new string
    arr->data[arr->len++] = str;
    return true;
}

void string_array_free(allocator* alloc, string_array_t* arr)
{
    if (!arr)
    {
        return;
    }

    alloc->free(arr, alloc->ctx);
}

// Convert string_array_t to const char** for C APIs like Vulkan
// Returns a contiguous array of char pointers for cache efficiency
const char** string_array_to_cstrings(const string_array_t* arr)
{
    if (!arr || arr->len == 0)
    {
        return NULL;
    }

    // Use a static buffer to avoid heap allocation for better performance
    // This assumes reasonable limits for typical use cases
    static const char* buffer[256];

    if (arr->len > 256)
    {
        return NULL;  // Safety check
    }

    for (size_t i = 0; i < arr->len; i++)
    {
        buffer[i] = arr->data[i].chars;
    }

    return buffer;
}

// Get const char* from string_t
const char* string_to_cstring(const string_t* str)
{
    return str ? str->chars : NULL;
}

typedef struct
{
    size_t len;
    size_t cap;
    void*  data[];
} list_t;

list_t* make_list(allocator* alloc, size_t len)
{
    // allocate twice the capacity, so if it grows
    // we dont have to re-allocate right away
    size_t buffer = 2;

    size_t  size     = sizeof(list_t) + len * sizeof(void*) * buffer;
    list_t* new_list = alloc->malloc(size, alloc->ctx);
    if (!new_list)
    {
        return nullptr;
    }

    new_list->len = 0;
    new_list->cap = len * 2;

    return new_list;
}

#endif  // PRELUDE_H
