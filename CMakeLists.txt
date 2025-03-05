cmake_minimum_required(VERSION 3.26)
project(Z3)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

#find_package(Vulkan REQUIRED)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3.8 # Choose your desired version tag
)

FetchContent_MakeAvailable(glfw)

#include_directories(${glfw_SOURCE_DIR}/include)
#include_directories(${Vulkan_INCLUDE_DIRS})

add_executable(Z3 src/main.cpp
        src/Engine.cpp
        src/MemoryManager.cpp
        src/WindowManager.cpp
        src/TimeManager.cpp
        src/InputManager.cpp
        src/RenderManager.cpp
        src/DynamicArray.cpp
        src/MaterialManager.cpp
        src/MaterialManager.h
        src/TextureManager.cpp
        src/TextureManager.h

)

if (DEFINED VULKAN_SDK_PATH)
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include") # 1.1 Make sure this include path is correct
    set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib") # 1.2 Make sure lib path is correct
    set(Vulkan_FOUND "True")
else()
    find_package(Vulkan REQUIRED) # throws error if could not find Vulkan
    message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
else()
    message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

target_include_directories(Z3 PRIVATE
        ${glfw_SOURCE_DIR}/include
        ${Vulkan_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(Z3 PRIVATE
        glfw
        ${Vulkan_LIBRARIES}
)