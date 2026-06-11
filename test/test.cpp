// Unused code, May be needed later
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

int main(){
glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
glm::mat4 trans = glm::mat4(1.0f);
trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
vec = trans * vec;
std::cout << vec.x << vec.y << vec.z << std::endl;
return 0;
}
//
// std::vector<Entity> loadGLBModelsFromDirectory(const std::string& directoryPath, std::shared_ptr<Shader>& shader){
//
//   std::vector<Entity> objects;
//   for (const auto& entry : std::filesystem::directory_iterator(directoryPath)){
//     if (!entry.is_regular_file()) {
//                 continue;
//             }
//     std::filesystem::path path = entry.path();
//     if(path.extension() == ".glb"){
//       auto model = std::make_shared<Model>(path);
//       Entity entity(model, shader);
//       objects.push_back(entity);
//     }
//   }
//   return objects;
// }

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
