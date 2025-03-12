#!/bin/bash

# Script to install all dependencies needed for building Z3

# Exit on error
set -e

echo "Installing Z3 build dependencies..."

# Update package lists
sudo apt-get update

# Install X11 RandR extension development files
sudo apt-get install -y libxrandr-dev

# Install additional X11 development packages that might be needed by GLFW
sudo apt-get install -y libxinerama-dev libxcursor-dev libxi-dev

# Install Vulkan development packages
sudo apt-get install -y libvulkan-dev

# Install OpenGL development packages
sudo apt-get install -y mesa-common-dev libgl1-mesa-dev

echo "All dependencies have been installed successfully!"
echo "You can now try building Z3 with CMake."