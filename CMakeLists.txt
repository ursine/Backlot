cmake_minimum_required(VERSION 3.20)
project(untitled)

set(CMAKE_CXX_STANDARD 17)
set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.224.1")

find_package(Vulkan REQUIRED)

include(FetchContent)
FetchContent_Declare(
        SDL2
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG "release-2.24.1"
)

FetchContent_GetProperties(SDL2)
if(NOT sdl2_POPULATED)
    FetchContent_Populate(SDL2)
    FetchContent_MakeAvailable(SDL2)
    message(STATUS "Catch source dir: ${sdl2_SOURCE_DIR}")
    message(STATUS "Catch binary dir: ${sdl2_BINARY_DIR}")
    add_subdirectory(${sdl2_SOURCE_DIR} ${sdl2_BINARY_DIR})
endif()

add_executable(untitled main.cpp utilities.h VulkanPP.h)
target_include_directories(untitled PUBLIC "${sdl2_SOURCE_DIR}/include" "${Vulkan_INCLUDE_DIR}")
target_link_libraries(untitled PRIVATE SDL2main SDL2-static Vulkan::Vulkan)

