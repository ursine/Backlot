cmake_minimum_required(VERSION 3.10)

# set the project name
project(Tutorial VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(Vulkan REQUIRED)

# add the executable
add_executable(Tutorial src/main.cpp src/vulkan.cc)

target_link_libraries(Tutorial PUBLIC Vulkan::Vulkan ${SDL2_LIBRARIES})

