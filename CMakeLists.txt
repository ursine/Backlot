cmake_minimum_required(VERSION 3.10)

# set the project name
project(Tutorial VERSION 1.0)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(Tutorial src/main.cpp)

target_link_libraries(Tutorial PUBLIC SDL2::SDL2 Vulkan::Vulkan)

