#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"
#include "mesh.hpp"
#include "texture.hpp"

struct RenderObject {
  std::shared_ptr<Shader> shader;
  std::shared_ptr<Mesh> mesh;
  const std::vector<std::shared_ptr<Texture>> textures;
};

class Entity{
  private:

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    RenderObject renderData; // Reference of mesh, shader and texture to use for the object

  public:
    Entity(std::shared_ptr<Mesh> meshAsset,
           std::shared_ptr<Shader> shaderAsset,
           std::vector<std::shared_ptr<Texture>> textures )
        : renderData{
              shaderAsset,
              meshAsset,
              std::move(textures)
          }{}    

    void translate(const glm::vec3& delta){ position = delta; }
    void rotate(const glm::vec3& delta){ rotation = delta; } 
    void setScale(const glm::vec3& s){ scale = s; }
    
    // Standard Physics Update
    void update(float deltaTime) {
    // Your simulation formulas go here (e.g., position calculation)
    }

    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        model = glm::scale(model, scale);
        return model;
    }

    // Read-only getter for the render loop
    const RenderObject& getRenderData() const { return renderData; }

    // Quick simulation helper methods
    void setPosition(const glm::vec3& pos) { position = pos; }

};
