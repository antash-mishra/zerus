//
// Created by akash on 6/7/25.
//

#ifndef SHADERS_H
#define SHADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <shaderc/shaderc.h>
#include "prelude.h"

#include <errno.h>
#include <string.h>

typedef enum shader_type
{
    VERTEX_SHADER,
    FRAGMENT_SHADER,
    GEOMETRY_SHADER,
    COMPUTE_SHADER
} shader_type;

// This function reads an entire file from provided path into a *single* `string_t`
static inline string_t* read_shader_file(allocator* alloc, const char* path)
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
    size_t total_size = sizeof(string_t) + file_size + 1;
    string_t* code = alloc->malloc(total_size, alloc->ctx);
    if (!code) {
        fclose(file);
        return NULL;
    }

    code->len = file_size;

    // The character buffer starts immediately after the string_t element in our single block.
    char* buffer = (char*)(code + 1);

    // The final string_t will still hold a const pointer, providing read-only access.
    code->chars = buffer;

    // Read the entire file into our allocated buffer.
    size_t bytes_read = fread(buffer, 1, file_size, file);
    fclose(file);

    if (bytes_read != (size_t)file_size) {
        // If we couldn't read the whole file, something is wrong. Free the memory and fail.
        alloc->free(code, alloc->ctx);
        return NULL;
    }

    // Manually add the null terminator at the end of the content.
    buffer[file_size] = '\0';

    return code;
}

// This function releases the resources of a compiled shader result.
void release_compiled_shader_result(shaderc_compilation_result_t compile_result, shaderc_compile_options_t compile_options, shaderc_compiler_t compiler) {
    shaderc_result_release(compile_result);
    shaderc_compile_options_release(compile_options);
    shaderc_compiler_release(compiler);
}

// This function compiles a GLSL shader to SPIR-V by running an external script.
void glsl_to_spirv(allocator* alloc, const char* glsl_path, const char* spirv_path, shader_type type)
{

    // read glsl file
    string_t* glsl_code = read_shader_file(alloc, glsl_path);

    // compile shader

    shaderc_compiler_t compiler = shaderc_compiler_initialize();
    shaderc_compile_options_t compile_options = shaderc_compile_options_initialize();
    shaderc_shader_kind kind;
    if (type == VERTEX_SHADER)
    {
        kind = shaderc_glsl_vertex_shader;
    }
    else if (type == FRAGMENT_SHADER)
    {
        kind = shaderc_glsl_fragment_shader;
    }
    else if (type == GEOMETRY_SHADER)
    {
        kind = shaderc_glsl_geometry_shader;
    }
    else if (type == COMPUTE_SHADER)
    {
        kind = shaderc_glsl_compute_shader;
    }
    else
    {
        fprintf(stderr, "Invalid shader type: %d\n", type);
        return;
    }

    shaderc_compilation_result_t compile_result = shaderc_compile_into_spv(
        compiler,
        glsl_code->chars,
        glsl_code->len,
        kind,
        glsl_path,
        "main",
        compile_options
    );

    // free string memory
    if (glsl_code)
    {
        alloc->free(glsl_code, alloc->ctx);
    }

    // check compilation errors
    if (shaderc_result_get_compilation_status(compile_result) != shaderc_compilation_status_success) {
        fprintf(stderr, "Error compiling shader %s: %s\n", glsl_path, shaderc_result_get_error_message(compile_result));
        release_compiled_shader_result(compile_result, compile_options, compiler);
        return;
    }

    size_t spirv_size = shaderc_result_get_length(compile_result);
    const char* spirv_bytes = shaderc_result_get_bytes(compile_result);

    // write to file
    FILE *fptr = fopen(spirv_path, "wb");
    if (!fptr)
    {
        fprintf(stderr, "Error opening file %s: %s\n", spirv_path, strerror(errno));
        release_compiled_shader_result(compile_result, compile_options, compiler);
        return;
    }
    fwrite(spirv_bytes, 1, spirv_size, fptr);
    fclose(fptr);

    release_compiled_shader_result(compile_result, compile_options, compiler);
}

#endif //SHADERS_H
