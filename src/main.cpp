#include <vector>
#include <math.h>
#include <memory>
#include <glm/gtx/io.hpp>

#include "entity.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "window.hpp"

int main(){
  // Initialize window first (creates the OpenGL context)
  Window window(1980, 1080, "Backpack");
  if (!window.init()) return -1;


  // Shader
  char vertexShaderSource[] = "../shaders/model_loading.vs";
  char fragmentShaderSource[] = "../shaders/model_loading.fs";
  auto shader_ptr = std::make_shared<Shader>(vertexShaderSource, fragmentShaderSource);
  
  // load models
  // -----------
  auto model_ptr = std::make_shared<Model>("../resources/backpack/backpack.obj");

  // Create objects
  // Mesh
  // Render Object
  Entity entity(
      model_ptr,
      shader_ptr);

  std::vector<Entity> objects;
  objects.push_back(entity);

  window.renderLoop(objects);

  return 0;
}
  // Element: Rectangle
  // std::vector<float> vertices = {
  //    0.5,  0.5, 0.0,  // top right
  //    0.5, -0.5, 0.0,  // bottom right
  //   -0.5, -0.5f, 0.0,  // bottom left
  //   -0.5,  0.5, 0.0   // top left 
  // };
  //
  // std::vector<int> indices = {  // note that we start from 0!
  //     0, 1, 3,   // first triangle
  //     1, 2, 3    // second triangle
  // };
 
  // Two triangles
  // std::vector<float> vertices1 = {
  //   -0.5, 0.0, 0.0,
  //   0.0,0.0, 0.0,
  //   0.0, 0.5, 0.0};
  // std::vector<float> vertices2 = {
  //   0.3, 0.0, 0.0,
  //   0.3, 0.5, 0.0,
  //   0.5, 0.0, 0.0
  // } ;


// Element: Rectangle with texture
  // std::vector<float> vertices = {
  //   // positions          // colors           // texture coords
  //    0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
  //    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
  //   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
  //   -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
  // };  
  //
  // std::vector<unsigned int> indices = {  
  //       0, 1, 3, // first triangle
  //       1, 2, 3  // second triangle
  //   };
