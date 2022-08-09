//
// Created by Gary Coulbourne on 8/6/22.
//

#include <cstdlib>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vulkan.hpp>

int main(int argc, char* argv[])
{
    VkExtent2D _windowExtent{ 1700 , 900 };

    SDL_Init(SDL_INIT_VIDEO);

    auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

    auto window = SDL_CreateWindow(
            "Vulkan Engine", //window title
            SDL_WINDOWPOS_UNDEFINED, //window position x (don't care)
            SDL_WINDOWPOS_UNDEFINED, //window position y (don't care)
            _windowExtent.width,  //window width in pixels
            _windowExtent.height, //window height in pixels
            window_flags
    );

    return EXIT_SUCCESS;
}