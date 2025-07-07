#include "engine/prelude.h"
#include "engine/shaders.h"
#include <stdlib.h> // For malloc/free
#include <stdio.h>
#include <stdbool.h>

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

// Helper to check if a file exists and is not empty
bool file_exists(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        return false;
    }
    fseek(file, 0, SEEK_END);
    bool is_not_empty = ftell(file) > 0;
    fclose(file);
    return is_not_empty;
}

int main()
{
    allocator std_alloc = {
        .malloc = std_malloc,
        .free = std_free,
        .ctx = NULL
    };

    // Paths must be relative to the build directory where the executable is run.
    const char* vert_path = "../resources/shaders/shadervs.vert";
    const char* frag_path = "../resources/shaders/shaderfs.frag";

    const char* vert_spv_path = "../resources/shaders/shadervs.spv";
    const char* frag_spv_path = "../resources/shaders/shaderfs.spv";

    // --- Compile Vertex Shader ---
    printf("--- Compiling Vertex Shader ---\n");
    glsl_to_spirv(&std_alloc, vert_path, vert_spv_path, VERTEX_SHADER);

    if (file_exists(vert_spv_path)) {
        printf("SUCCESS: '%s' was created.\n", vert_spv_path);
    } else {
        printf("FAILURE: '%s' was not created.\n", vert_spv_path);
    }

    // --- Compile Fragment Shader ---
    printf("\n--- Compiling Fragment Shader ---\n");
    glsl_to_spirv(&std_alloc, frag_path, frag_spv_path, FRAGMENT_SHADER);

    if (file_exists(frag_spv_path)) {
        printf("SUCCESS: '%s' was created.\n", frag_spv_path);
    } else {
        printf("FAILURE: '%s' was not created.\n", frag_spv_path);
    }

    return 0;
}
