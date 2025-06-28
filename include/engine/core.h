/*
 * Zerus Game Engine - Core Module
 *
 * A cross-platform game engine core system
 *
 * LICENSE:
 * This software is dual-licensed to the public domain and under the following
 * license: you are granted a perpetual, irrevocable license to copy, modify,
 * publish, and distribute this file as you see fit.
 *
 * USAGE:
 * Do this:
 *    #define ZERUS_CORE_IMPLEMENTATION
 * before you include this file in *one* C or C++ file to create the
 * implementation.
 *
 * // i.e. it should look like this:
 * #include ...
 * #include ...
 * #include ...
 * #define ZERUS_CORE_IMPLEMENTATION
 * #include "engine/core.h"
 *
 * You can #define ZERUS_CORE_STATIC before the #include to make the
 * implementation private to the source file that creates it.
 */

#ifndef ZERUS_CORE_H
#define ZERUS_CORE_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>


// Engine version
#define ENGINE_VERSION_MAJOR 1
#define ENGINE_VERSION_MINOR 0
#define ENGINE_VERSION_PATCH 0

// Configuration
#ifndef ZERUS_CORE_DEF
#ifdef ZERUS_CORE_STATIC
#define ZERUS_CORE_DEF static
#else
#define ZERUS_CORE_DEF extern
#endif
#endif

typedef struct string_slice_t
{
    const char** data;
    size_t       len;
} string_slice_t;

typedef enum
{
    INIT_OK,
    VULKAN_INSTANCE_FAILED,
} engine_error_t;

// Engine subsystems state
typedef struct zerus_engine_state_t
{
    bool           initialized;
    engine_error_t err;
    VkInstance     instance;
} zerus_engine_state_t;


// Core engine functions
ZERUS_CORE_DEF zerus_engine_state_t zerus_engine_init(string_slice_t);
ZERUS_CORE_DEF void zerus_engine_update(zerus_engine_state_t* engine);
ZERUS_CORE_DEF void zerus_engine_shutdown(zerus_engine_state_t* engine);


#endif  // ZERUS_CORE_H

//
// IMPLEMENTATION
//

#ifdef ZERUS_CORE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"


// Private namespace for internal functions
#define zerus_core__ zerus_core__


engine_error_t _init_vulkan(zerus_engine_state_t engine,
                            string_slice_t       extension_data)
{
    // create instance
    const VkApplicationInfo app_info = {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext              = NULL,
        .pApplicationName   = "Zerus Engine",
        .applicationVersion = VK_MAKE_VERSION(
            ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH),
        .engineVersion = VK_MAKE_VERSION(
            ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH),
        .apiVersion = VK_API_VERSION_1_4,
    };

    const VkInstanceCreateInfo instance_create_info
        = { .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext                   = NULL,
            .pApplicationInfo        = &app_info,
            .enabledExtensionCount   = extension_data.len,
            .ppEnabledExtensionNames = extension_data.data,
            .enabledLayerCount       = 0,
            .ppEnabledLayerNames     = nullptr };

    VkResult result
        = vkCreateInstance(&instance_create_info, NULL, &engine.instance);
    if (result)
    {
        printf("error creating vulkan instance");
        return VULKAN_INSTANCE_FAILED;
    }


    printf("Vulkan instance created...\n");
    return INIT_OK;
}

ZERUS_CORE_DEF
zerus_engine_state_t zerus_engine_init(string_slice_t extension_data)
{
    zerus_engine_state_t state = { .initialized = true };

    // Initialize subsystems
    printf("Initializing rendessrer... \n");
    state.err = _init_vulkan(state, extension_data);
    if (state.err)
    {
        printf("error in vulkan init %d", state.err);
        return state;
    }

    return state;
}

ZERUS_CORE_DEF void zerus_engine_update(zerus_engine_state_t*)
{
    // Update all engine subsystems
    // This would typically include:
    // - Input processing
    // - Physics simulation
    // - Audio processing
    // - Rendering

    // Placeholder for now
}

ZERUS_CORE_DEF void zerus_engine_shutdown(zerus_engine_state_t* engine)
{
    printf("Shutting down subsystems...\n");

    if (engine->initialized)
    {
        vkDestroyInstance(engine->instance, nullptr);

        engine->initialized = false;
    }
}


#endif  // ZERUS_CORE_IMPLEMENTATION
