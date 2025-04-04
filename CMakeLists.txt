cmake_minimum_required(VERSION 3.16)
project(Z3)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

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

#find_package(Vulkan REQUIRED)
find_package(nlohmann_json 3.11.3 REQUIRED)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3.8 # Choose your desired version tag
)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v1.91.1  # Choose your desired version tag
)


FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
        src/game/editor/LevelEditor.cpp
        src/game/editor/LevelEditor.h
        src/game/editor/myImgui_LevelEditor.cpp
        src/game/editor/myImgui_LevelEditor.h
        src/game/components/Component.cpp
        src/game/components/Component.h
        src/game/components/TransformComponent.cpp
        src/game/components/TransformComponent.h
        src/game/components/PhysicsComponent.cpp
        src/game/components/PhysicsComponent.h
        src/game/editor/SerializationHelper.h
        src/game/components/RenderComponent.cpp
        src/game/components/RenderComponent.h
        src/game/components/CollisionComponent.cpp
        src/game/components/CollisionComponent.h
)

target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${glfw_SOURCE_DIR}/include
        ${Vulkan_INCLUDE_DIRS}
)

#include_directories(${glfw_SOURCE_DIR}/include)
#include_directories(${Vulkan_INCLUDE_DIRS})

#target_compile_definitions(imgui PUBLIC IMGUI_IMPL_VULKAN_NO_PROTOTYPES)
target_link_libraries(imgui PUBLIC glfw ${Vulkan_LIBRARIES} nlohmann_json::nlohmann_json)


add_executable(Z3 src/main.cpp
        src/Engine.cpp
        src/MemoryManager.cpp
        src/WindowManager.cpp
        src/TimeManager.cpp
        src/InputManager.cpp
        src/RenderManager.cpp
        src/DynamicArray.cpp
        src/GameManager.cpp
        src/GameManager.h
        src/game/GameObject.cpp
        src/game/GameObject.h
        src/game/Material.cpp
        src/game/Material.h
        src/game/Texture.cpp
        src/game/Texture.h
        src/game/Mesh.cpp
        src/game/Mesh.h
        src/tools/MaterialFactory.cpp
        src/tools/MaterialFactory.h
        src/tools/TextureFactory.cpp
        src/tools/TextureFactory.h
        src/tools/MeshFactory.cpp
        src/tools/MeshFactory.h
        src/game/UniformBufferObject.h
        src/game/ModelPushConstant.h
        src/game/entities/Entity.cpp
        src/game/entities/Entity.h
        src/tools/EntityFactory.cpp
        src/tools/EntityFactory.h
        src/game/entities/EntityTypes.cpp
        src/game/entities/EntityTypes.h
        src/game/entities/PlayerEntity.cpp
        src/game/entities/PlayerEntity.h
        src/game/CameraManager.cpp
        src/game/CameraManager.h
        src/game/MouseManager.cpp
        src/game/MouseManager.h
        src/MyImgui.cpp
        src/MyImgui.h
        src/game/entities/FloorEntity.cpp
        src/game/entities/FloorEntity.h
        src/tools/ComponentFactory.cpp
        src/tools/ComponentFactory.h
        src/game/entities/GrandPianoEntity.cpp
        src/game/entities/GrandPianoEntity.h
        src/tools/PhysicsFactory.cpp
        src/tools/PhysicsFactory.h
        src/PhysicsTest.cpp
        src/PhysicsTest.h
        src/PhysicsManager.cpp
        src/PhysicsManager.h

)


target_compile_definitions(Z3 PRIVATE IMGUI=1 DEBUG=1)







target_include_directories(Z3 PRIVATE
        src/
        ${glfw_SOURCE_DIR}/include
        ${Vulkan_INCLUDE_DIRS}
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

# Link libraries
target_link_libraries(Z3 PRIVATE
        glfw
        imgui
        ${Vulkan_LIBRARIES}
)
