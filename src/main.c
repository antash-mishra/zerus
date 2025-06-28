#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ZERUS_CORE_IMPLEMENTATION
#include "engine/core.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <cglm/cglm.h>

GLFWwindow* make_window()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    return glfwCreateWindow(800, 600, "Zerus", NULL, NULL);
}

string_slice_t get_glfw_extensions()
{
    uint32_t     count           = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&count);

    return (string_slice_t) {
        glfw_extensions,
        count,
    };
}

int main(int argc, char* argv[])
{
    (void) argc;  // Suppress unused parameter warning
    (void) argv;

    printf("Zerus Game Engine v1.0.0\n");
    printf("Initializing engine...\n");

    glfwInit();

    zerus_engine_state_t engine = zerus_engine_init(get_glfw_extensions());
    if (!engine.initialized)
    {
        fprintf(stderr, "Failed to initialize engine\n");
        return EXIT_FAILURE;
    }

    printf("Engine initialized successfully\n");

    printf("Initializing window \n");
    GLFWwindow* window = make_window();

    // Main game loop would go here
    bool running = true;
    while (running)
    {
        if (glfwWindowShouldClose(window))
        {
            running = false;
            break;
        }

        glfwPollEvents();

        // Update engine systems
        zerus_engine_update(&engine);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    zerus_engine_shutdown(&engine);
    printf("Engine shutdown complete\n");

    return EXIT_SUCCESS;
}
