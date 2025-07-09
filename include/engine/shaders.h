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
    string_t* glsl_code = read_file(alloc, glsl_path);

    // compile shader
    shaderc_compiler_t compiler = shaderc_compiler_initialize();
    shaderc_compile_options_t compile_options = shaderc_compile_options_initialize();
    shaderc_shader_kind kind;
    switch (type) {
        case VERTEX_SHADER:   kind = shaderc_glsl_vertex_shader;   break;
        case FRAGMENT_SHADER: kind = shaderc_glsl_fragment_shader; break;
        case GEOMETRY_SHADER: kind = shaderc_glsl_geometry_shader; break;
        case COMPUTE_SHADER:  kind = shaderc_glsl_compute_shader;  break;
        default:
            fprintf(stderr, "Invalid shader type: %d\n", type);
            // Must release the resources we've already acquired before returning.
            shaderc_compile_options_release(compile_options);
            shaderc_compiler_release(compiler);
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

    // spirv returned shader with size
    size_t spirv_size = shaderc_result_get_length(compile_result);
    const char* spirv_bytes = shaderc_result_get_bytes(compile_result);

    // write to file
    bool success = write_file(spirv_path, spirv_bytes, spirv_size);
    if (!success) {
        fprintf(stderr, "Error writing SPIR-V file %s\n", spirv_path);
    }

    // release memory of shader result
    release_compiled_shader_result(compile_result, compile_options, compiler);
}

#endif //SHADERS_H
