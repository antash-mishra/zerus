//
// Created by akash on 6/7/25.
//

#ifndef SHADERS_H
#define SHADERS_H

#include <stdio.h>
#include <stdlib.h>
#include "prelude.h"

#include <errno.h>

// This function reads an entire file from provided path into a *single* `string_t` which is
// stored inside a `string_array_t`.
static inline string_array_t* read_shader_file(allocator* alloc, const char* path)
{
    FILE *file = fopen(path, "rb");
    if (!file) {

        return NULL;
    }

    // Seek to the end of the file to determine its size.
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size < 0) {
        fclose(file);
        return NULL;
    }

    // Allocate one contiguous memory block for the header, the string_t view,
    // the file content, and a null terminator. This makes cleanup trivial.
    size_t total_size = sizeof(string_array_t) + sizeof(string_t) + file_size + 1;
    string_array_t* arr = alloc->malloc(total_size, alloc->ctx);
    if (!arr) {
        fclose(file);
        return NULL;
    }

    // This string array will always contain exactly one element: the file content.
    arr->len = 1;
    arr->cap = 1;

    string_t* str = arr->data;
    str->len = file_size;

    // The character buffer starts immediately after the string_t element in our single block.
    char* buffer = (char*)(arr->data + 1);

    // The final string_t will still hold a const pointer, providing read-only access.
    str->chars = buffer;

    // Read the entire file into our allocated buffer.
    size_t bytes_read = fread(buffer, 1, file_size, file);
    fclose(file);

    if (bytes_read != (size_t)file_size) {
        // If we couldn't read the whole file, something is wrong. Free the memory and fail.
        alloc->free(arr, alloc->ctx);
        return NULL;
    }

    // Manually add the null terminator at the end of the content.
    buffer[file_size] = '\0';

    return arr;
}

// This function compiles a GLSL shader to SPIR-V by running an external script.
static inline string_array_t* glsl_to_spirv(allocator* alloc, const char* glsl_path, const char* spirv_path)
{

    // Build the full command to execute the compilation script.
    char command[2048];
    snprintf(command, sizeof(command), "../compile.sh %s %s", glsl_path, spirv_path);

    printf("Executing: %s\n", command);
    int result = system(command);

    if (result != 0) {
        fprintf(stderr, "Error: compile.sh script failed for '%s'.\n", glsl_path);
        return NULL;
    }

    // If compilation succeeded, load the resulting SPIR-V binary from disk.
    printf("Script finished successfully. Loading '%s'...\n", spirv_path);
    return read_shader_file(alloc, spirv_path);
}

#endif //SHADERS_H
