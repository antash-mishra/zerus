//
// Created by akash on 5/7/25.
//

#ifndef SURFACE_H
#define SURFACE_H
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include "prelude.h"
#include "GLFW/glfw3.h"

typedef enum
{
    SURFACE_OK,
    SURFACE_SHOULD_CLOSE
} surface_status_t;


GLFWwindow* make_window()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    return glfwCreateWindow(800, 600, "Zerus", NULL, NULL);
}

string_array_t* get_glfw_extensions(allocator* alloc)
{
    uint32_t     count           = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&count);

    string_array_t* extensions = make_string_array(alloc, count + 1);

    for (uint32_t i = 0; i < count; i++)
    {
        extensions->data[i] = make_from_c_string(glfw_extensions[i]);
        extensions->len++;
    }

    return extensions;
}

typedef struct
{
    surface_status_t status;
    VkSurfaceKHR     surface;
    GLFWwindow*      window;
} surface_info_t;

surface_info_t create_surface(VkInstance)
{
    GLFWwindow* window = make_window();

    return (surface_info_t) {
        .status  = SURFACE_OK,
        .window  = window,
        .surface = VK_NULL_HANDLE,
    };
}

surface_status_t update_surface(surface_info_t* surface)
{
    if (glfwWindowShouldClose(surface->window))
    {
        return SURFACE_SHOULD_CLOSE;
    }

    glfwPollEvents();

    return SURFACE_OK;
}

void destroy_surface(surface_info_t* surface)
{
    glfwDestroyWindow(surface->window);
    glfwTerminate();
}

#endif  // SURFACE_H
