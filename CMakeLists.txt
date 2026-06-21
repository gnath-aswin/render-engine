cmake_minimum_required(VERSION 3.10)

project(Graphics)

set(CMAKE_CXX_STANDARD 17)

# 1. Enable this so Neovim/clangd can see your Assimp and OpenGL include paths
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 2. Locate the installed Assimp package on your Ubuntu system
find_package(assimp REQUIRED)
set(CMAKE_PREFIX_PATH
    "${CMAKE_SOURCE_DIR}/external/libtorch"
    ${CMAKE_PREFIX_PATH}
)
find_package(Torch REQUIRED)

add_executable(app
  src/glad.c
  src/window.cpp
  src/shader.cpp
  src/mesh.cpp
  src/texture.cpp
  src/main.cpp
  src/mujoco_simulation.cpp
  src/mujoco_robot_loader.cpp
  src/g1_humanoid.cpp
  src/gaussian_splat_model.cpp
  src/gaussian_background_renderer.cpp
  test/test_torch.cpp
  src/g1_walking_policy.cpp
)
target_include_directories(app PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/external/mujoco/include
  ${CMAKE_SOURCE_DIR}/external/libtorch/include
)

target_link_libraries(app PRIVATE
  glfw
  ${CMAKE_DL_LIBS}
  assimp::assimp
  tinyxml2
  ${CMAKE_SOURCE_DIR}/external/mujoco/lib/libmujoco.so.3.9.0
  ${TORCH_LIBRARIES}
)

set_target_properties(app PROPERTIES
  BUILD_RPATH "${CMAKE_SOURCE_DIR}/external/mujoco/lib"
)

target_compile_options(app PRIVATE
    ${TORCH_CXX_FLAGS}
)
