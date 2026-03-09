cmake_minimum_required(VERSION 3.16)
project(GameEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optionally group targets in IDE
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add GLFW (uses its own CMakeLists.txt)
add_subdirectory(external/glfw)

# Add engine and sandbox
add_subdirectory(engine)
add_subdirectory(sandbox)