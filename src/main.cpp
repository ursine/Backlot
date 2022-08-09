//
// Created by Gary Coulbourne on 8/6/22.
//

#include <cstdlib>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vulkan/vk_icd.h>

int main(int argc, char* argv[])
{
    VkExtent2D _windowExtent{ 640 , 400 };

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

    SDL_Event e;
    bool bQuit = false;

    //main loop
    while (!bQuit)
    {
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            //close the window when user clicks the X button or alt-f4s
            if (e.type == SDL_QUIT) bQuit = true;
        }

        //draw();
    }

    return EXIT_SUCCESS;
}