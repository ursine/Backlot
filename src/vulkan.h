//
// Created by Gary on 8/12/22.
//

#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

class Vulkan {
private:
    // Passed to the outside
    std::shared_ptr<VkInstance> vulkan_instance;
    std::unique_ptr<std::vector<VkExtensionProperties>> vulkan_extensions;

    // Internal tooling, used only inside

public:
    // Singleton Instance
    static Vulkan& instance() {
        static Vulkan instance;
        return instance;
    }

    [[nodiscard]] std::shared_ptr<VkInstance> vulkan() { return vulkan_instance; }


    // We don't need these so nuke 'em
    Vulkan(Vulkan const&)         = delete;
    void operator=(Vulkan const&) = delete;

    ~Vulkan() {}


private:
    Vulkan(): vulkan_instance(std::make_shared<VkInstance>()) {
        VkResult result = VK_SUCCESS;

        // Dig out the vulkan extensions
        uint32_t count;
        result = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
        if (result != VK_SUCCESS) {
            // Die

        }

        std::vector<const char*> extensionNames(0);

        const std::vector<const char*> validationLayers = {
                ///has bug
                //"VK_LAYER_LUNARG_standard_validation"
        };

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "unknown";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_MAKE_VERSION(1,0,0);

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo;
        instanceCreateInfo.enabledLayerCount = validationLayers.size();
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
        instanceCreateInfo.enabledExtensionCount = extensionNames.size();
        instanceCreateInfo.ppEnabledExtensionNames = extensionNames.data();

        result = vkCreateInstance(&instanceCreateInfo, nullptr, vulkan_instance.get());
    }
};

/*
 *
 *
typedef enum VkResult {
    VK_SUCCESS = 0,
    VK_NOT_READY = 1,
    VK_TIMEOUT = 2,
    VK_EVENT_SET = 3,
    VK_EVENT_RESET = 4,
    VK_INCOMPLETE = 5,
    VK_ERROR_OUT_OF_HOST_MEMORY = -1,
    VK_ERROR_OUT_OF_DEVICE_MEMORY = -2,
    VK_ERROR_INITIALIZATION_FAILED = -3,
    VK_ERROR_DEVICE_LOST = -4,
    VK_ERROR_MEMORY_MAP_FAILED = -5,
    VK_ERROR_LAYER_NOT_PRESENT = -6,
    VK_ERROR_EXTENSION_NOT_PRESENT = -7,
    VK_ERROR_FEATURE_NOT_PRESENT = -8,
    VK_ERROR_INCOMPATIBLE_DRIVER = -9,
    VK_ERROR_TOO_MANY_OBJECTS = -10,
    VK_ERROR_FORMAT_NOT_SUPPORTED = -11,
    VK_ERROR_FRAGMENTED_POOL = -12,
    VK_ERROR_OUT_OF_POOL_MEMORY = -1000069000,
    VK_ERROR_INVALID_EXTERNAL_HANDLE = -1000072003,
    VK_ERROR_SURFACE_LOST_KHR = -1000000000,
    VK_ERROR_NATIVE_WINDOW_IN_USE_KHR = -1000000001,
    VK_SUBOPTIMAL_KHR = 1000001003,
    VK_ERROR_OUT_OF_DATE_KHR = -1000001004,
    VK_ERROR_INCOMPATIBLE_DISPLAY_KHR = -1000003001,
    VK_ERROR_VALIDATION_FAILED_EXT = -1000011001,
    VK_ERROR_INVALID_SHADER_NV = -1000012000,
    VK_ERROR_NOT_PERMITTED_EXT = -1000174001,
    VK_ERROR_OUT_OF_POOL_MEMORY_KHR = VK_ERROR_OUT_OF_POOL_MEMORY,
    VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR = VK_ERROR_INVALID_EXTERNAL_HANDLE,
    VK_RESULT_BEGIN_RANGE = VK_ERROR_FRAGMENTED_POOL,
    VK_RESULT_END_RANGE = VK_INCOMPLETE,
    VK_RESULT_RANGE_SIZE = (VK_INCOMPLETE - VK_ERROR_FRAGMENTED_POOL + 1),
    VK_RESULT_MAX_ENUM = 0x7FFFFFFF
} VkResult;

 *
 *
 */