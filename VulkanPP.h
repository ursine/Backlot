//
// Created by bear on 10/19/2022.
//

#pragma once

#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>


#define CHECK_VULKAN(FN) \
{ \
    VkResult r = FN; \
    if (r != VK_SUCCESS) {\
        std::cout << #FN << " failed\n   " << (int)r << " @ " << __LINE__ << std::flush; \
        throw std::runtime_error(#FN " failed!");  \
    } \
}


class VulkanPP {
private:
    SDL_Window* const window = nullptr;

public:
    explicit VulkanPP(SDL_Window* const w): window(w) {

    }
};