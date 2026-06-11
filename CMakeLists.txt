cmake_minimum_required(VERSION 3.10)

project(Graphics)

set(CMAKE_CXX_STANDARD 17)

# 1. Enable this so Neovim/clangd can see your Assimp and OpenGL include paths
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

# 2. Locate the installed Assimp package on your Ubuntu system
find_package(assimp REQUIRED)

add_executable(app
  src/glad.c
  src/window.cpp
  src/shader.cpp
  src/mesh.cpp
  src/texture.cpp
  src/main.cpp
  src/mujoco_robot_loader.cpp
  src/gaussian_splat_model.cpp
)

# 3. Link Assimp to your executable alongside glfw and system libraries
target_link_libraries(app PRIVATE 
  glfw 
  ${CMAKE_DL_LIBS}
  assimp::assimp
  tinyxml2
)
