# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands
- Build: `/Applications/CLion.app/Contents/bin/cmake/mac/x64/bin/cmake --build /Users/socki/dev/z3/cmake-build-debug --target Z3 -- -j 14`
- Run: `./z3` (from build directory)
- Dependencies: Run `./codespaces_script_dependencies.sh` to install required packages on Ubuntu/Debian

## Code Style Guidelines
- Naming: Classes/Structs (PascalCase), Methods (PascalCase), Variables (camelCase), Constants (ALL_CAPS)
- Types: Custom aliases (r32/f32 for float, etc.), Math types (vec2, vec3, etc.)
- Headers: Use include guards (Z3_FILENAME_H), not #pragma once
- Functions: Init/Update prefixes for lifecycle methods, pass-by-pointer for modifiable objects
- Formatting: 4-space indentation, braces on new line for functions, same line for control flow
- Architecture: Component-based design with Entity-Component system
- Math: Use vector constructors (V2, V3, V4) and utility functions from math.h

## Project Structure
- src/: Core engine (managers, utilities)
- src/game/: Game logic, components, entities
- src/math/: Vector/matrix math library
- src/render/: Models, shaders, textures
- src/tools/: Factory classes

Always follow existing code patterns when adding new features or components.