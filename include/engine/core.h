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
#include "prelude.h"


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

typedef enum
{
    INIT_OK,
    VULKAN_INSTANCE_FAILED,
    VULKAN_VALIDATION_NOT_FOUND
} engine_error_t;

// Engine subsystems state
typedef struct zerus_engine_state_t
{
    bool           initialized;
    engine_error_t err;

    VkInstance               instance;
    VkDebugUtilsMessengerEXT debug_messenger;
} zerus_engine_state_t;


// Core engine functions
ZERUS_CORE_DEF zerus_engine_state_t zerus_engine_init(allocator*,
                                                      string_array_t*);
ZERUS_CORE_DEF void zerus_engine_update(zerus_engine_state_t* engine);
ZERUS_CORE_DEF void zerus_engine_shutdown(zerus_engine_state_t* engine);


#endif  // ZERUS_CORE_H

//
// IMPLEMENTATION
//

#ifdef ZERUS_CORE_IMPLEMENTATION

#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"


// Private namespace for internal functions
#define zerus_core__ zerus_core__

string_t required_validation_layer = MAKE_STR("VK_LAYER_KHRONOS_validation");
string_t required_validation_extension
    = MAKE_STR(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);


bool check_validation_support()
{
    uint32_t count;
    vkEnumerateInstanceLayerProperties(&count, NULL);

    VkLayerProperties layer_properties[256];
    vkEnumerateInstanceLayerProperties(&count, layer_properties);

    bool found = false;
    for (uint32_t i = 0; i < count; i++)
    {
        VkLayerProperties layer = layer_properties[i];

        string_t layer_name
            = { .chars = layer.layerName,
                .len   = find_length_of_c_string(layer.layerName) };


        if (string_equal(layer_name, required_validation_layer))
        {
            found = true;
            break;
        }
    }

    return found;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
               VkDebugUtilsMessageTypeFlagsEXT             message_type,
               const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
               void*                                       p_user_data)
{
    (void) p_user_data;

    printf("message severity %d \n", message_severity);
    printf("message type: %d \n", message_type);
    printf("validation layer: %s \n", p_callback_data->pMessage);
    return VK_FALSE;
}

bool create_debug_utils_messenger(VkInstance                instance,
                                  VkDebugUtilsMessengerEXT* debug_messenger)
{
    VkDebugUtilsMessengerCreateInfoEXT create_info = { 0 };
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    create_info.messageSeverity
        = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
          | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                              | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                              | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    create_info.pfnUserCallback = debug_callback;
    create_info.pUserData       = NULL;

    PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugUtilsMessengerEXT
        = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            instance, "vkCreateDebugUtilsMessengerEXT");
    if (pfnCreateDebugUtilsMessengerEXT == NULL)
    {
        printf("failed to register debug callback");
        return false;
    }

    VkResult res = pfnCreateDebugUtilsMessengerEXT(
        instance, &create_info, nullptr, debug_messenger);
    if (res)
    {
        printf("failed to create debug messenger %d", res);
        return false;
    }

    return true;
}

void destroy_debug_utils_messenger(VkInstance               instance,
                                   VkDebugUtilsMessengerEXT debug_messenger)
{
    PFN_vkDestroyDebugUtilsMessengerEXT pfnDestroyDebugUtilsMessengerEXT
        = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            instance, "vkDestroyDebugUtilsMessengerEXT");
    if (pfnDestroyDebugUtilsMessengerEXT == NULL)
    {
        return;
    }

    pfnDestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
}


engine_error_t _init_vulkan(allocator*            alloc,
                            zerus_engine_state_t* engine,
                            string_array_t*       extension_data)
{
    if (!check_validation_support())
    {
        printf("validation support not found");
        return VULKAN_VALIDATION_NOT_FOUND;
    }

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


    string_array_push(alloc, extension_data, required_validation_extension);

    const VkInstanceCreateInfo instance_create_info
        = { .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext                   = NULL,
            .pApplicationInfo        = &app_info,
            .enabledExtensionCount   = extension_data->len,
            .ppEnabledExtensionNames = string_array_to_cstrings(extension_data),
            .enabledLayerCount       = 1,
            .ppEnabledLayerNames     = (const char*[]) {
                string_to_cstring(&required_validation_layer) } };

    VkResult result
        = vkCreateInstance(&instance_create_info, NULL, &engine->instance);
    if (result)
    {
        printf("error creating vulkan instance");
        return VULKAN_INSTANCE_FAILED;
    }

    if (!create_debug_utils_messenger(engine->instance,
                                      &engine->debug_messenger))
    {
        return VULKAN_VALIDATION_NOT_FOUND;
    }

    // create device
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(engine->instance, &device_count, nullptr);

    // VkPhysicalDevice* physical_devices
    //     = alloc->malloc(device_count * sizeof(VkPhysicalDevice), alloc->ctx);


    list_t* physical_devices = make_list(alloc, device_count);

    if (!physical_devices)
    {
        return VULKAN_INSTANCE_FAILED;
    }

    vkEnumeratePhysicalDevices(engine->instance,
                               &device_count,
                               (VkPhysicalDevice*) &physical_devices->data);

    for (uint32_t i = 0; i < device_count; i++)
    {
        VkPhysicalDevice* device = physical_devices->data[i];

        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(*device, &props);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(*device, &features);

        printf("device %d, name %s, type: %d %d\n",
               props.deviceID,
               props.deviceName,
               props.deviceType,
               features.geometryShader);
    }

    // Clean up physical devices array
    alloc->free(physical_devices, alloc->ctx);

    printf("Vulkan instance created...\n");
    return INIT_OK;
}

ZERUS_CORE_DEF
zerus_engine_state_t zerus_engine_init(allocator*      alloc,
                                       string_array_t* extension_data)
{
    zerus_engine_state_t state = { .initialized = true };

    // Initialize subsystems
    printf("Initializing rendessrer... \n");
    state.err = _init_vulkan(alloc, &state, extension_data);
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
        destroy_debug_utils_messenger(engine->instance,
                                      engine->debug_messenger);

        vkDestroyInstance(engine->instance, nullptr);

        engine->initialized = false;
    }
}


#endif  // ZERUS_CORE_IMPLEMENTATION
