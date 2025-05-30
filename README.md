# Z3 Engine

## Building on macOS

Building on macOS requires the Vulkan SDK to be installed. You can download it from the official LunarG website.

CMake needs to be able to find the Vulkan SDK. If `find_package(Vulkan)` fails, you might need to set the `VULKAN_SDK` environment variable (e.g., `export VULKAN_SDK=/path/to/vulkansdk/macOS`) or pass the path to CMake directly (e.g., `cmake -DVULKAN_SDK_PATH=/path/to/vulkansdk/macOS ..`). Consult the `CMakeLists.txt` for more details on how the SDK path is handled.

**Important Note:** Currently, there is a known issue where the Clang compiler (tested with version 18.1.3) crashes with a segmentation fault when attempting to compile `src/game/editor/LevelEditor.cpp`. This prevents a full, clean build of the project on macOS at this time. This issue needs further investigation in a native macOS environment. While changes for Vulkan portability on macOS have been included, this compiler problem is a significant blocker.
