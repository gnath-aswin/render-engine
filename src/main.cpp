#include <string>
#include <vector>
#include <math.h>
#include <memory>
#include <glm/gtx/io.hpp>

#include "cube_data.hpp"
#include "entity.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "window.hpp"

int main(){
  // Initialize window first (creates the OpenGL context)
  Window window(1980, 1080, "AslimCubism");
  if (!window.init()) return -1;

  // Create objects
  // Mesh
  std::vector<unsigned int> indices;
  auto mesh_ptr = std::make_shared<Mesh>(
      vertices,
      indices,
      6,
      std::vector<VertexAttribute>{
          {0, 3, 0}, // position
          {1, 2, 3}, // tex coord
          {2, 1, 5}  // texture index
      }
  );

  // Shader
  char vertexShaderSource[] = "../shaders/multiple_tex.vs";
  char fragmentShaderSource[] = "../shaders/multiple_tex.fs";
  auto shader_ptr = std::make_shared<Shader>(vertexShaderSource, fragmentShaderSource);

  // Texture
  std::vector<std::shared_ptr<Texture>> textures;
  for(int i=0; i<6; ++i){
    std::string texture_path = "../resources/textures/asli" + std::to_string(i) + ".jpeg";
    std::string texture_name = "texture" + std::to_string(i + 1);
    auto texture = std::make_shared<Texture>(texture_path.c_str(), texture_name); 
    textures.push_back(texture);
  }

  // Render Object
  Entity entity(
      mesh_ptr,
      shader_ptr,
      textures
  );
  // Mulitple object
  std::vector<glm::vec3> cubePositions = {
      glm::vec3( 0.0f,  0.0f,  0.0f), 
      glm::vec3( 2.0f,  5.0f, -15.0f), 
      glm::vec3(-1.5f, -2.2f, -2.5f),  
      glm::vec3(-3.8f, -2.0f, -12.3f),  
      glm::vec3( 2.4f, -0.4f, -3.5f),  
      glm::vec3(-1.7f,  3.0f, -7.5f),  
      glm::vec3( 1.3f, -2.0f, -2.5f),  
      glm::vec3( 1.5f,  2.0f, -2.5f), 
      glm::vec3( 1.5f,  0.2f, -1.5f), 
      glm::vec3(-1.3f,  1.0f, -1.5f)  
  };
  std::vector<Entity> objects;
  for(int i=0; i<cubePositions.size(); ++i){
    entity.setPosition(cubePositions[i]);
    objects.push_back(entity);
  }
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
