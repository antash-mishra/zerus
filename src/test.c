#include "engine/prelude.h"
#include "engine/shaders.h"
#include <stdlib.h> // For malloc/free
#include <stdio.h>

// Standard library allocator wrappers
static void* std_malloc(ptrdiff_t size, void* ctx)
{
    (void) ctx;
    return malloc(size);
}

static void std_free(void* ptr, void* ctx)
{
    (void) ctx;
    free(ptr);
}

int main()
{
    allocator std_alloc = {
        .malloc = std_malloc,
        .free = std_free,
        .ctx = NULL
    };

    // Paths must be relative to the build directory where the executable is run.
    const char* vert_path = "../resources/shaders/shaderfs.frag";
    const char* frag_path = "../resources/shaders/shadervs.vert";

    const char* vert_spv_path = "../resources/shaders/vert.spv";
    const char* frag_spv_path = "../resources/shaders/frag.spv";

    // --- Compile and Load Vertex Shader ---
    printf("--- Processing Vertex Shader ---\n");
    string_array_t* vert_shader = glsl_to_spirv(&std_alloc, vert_path, vert_spv_path);
    if (vert_shader) {
        printf("Loaded %zu bytes from %s\n", vert_shader->data[0].len, vert_spv_path);
        string_array_free(&std_alloc, vert_shader);
    } else {
        printf("Failed to process vertex shader.\n");
    }

    // --- Compile and Load Fragment Shader ---
    printf("\n--- Processing Fragment Shader ---\n");
    string_array_t* frag_shader = glsl_to_spirv(&std_alloc, frag_path, frag_spv_path);
    if (frag_shader) {
        printf("Loaded %zu bytes from %s\n", frag_shader->data[0].len, frag_spv_path);
        string_array_free(&std_alloc, frag_shader);
    } else {
        printf("Failed to process fragment shader.\n");
    }

    return 0;
}
