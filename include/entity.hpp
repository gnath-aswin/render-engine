#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "node.hpp"
#include "shader.hpp"

enum class RenderType{
  Mesh,
  Gaussian
};

class Entity {
private:
    std::string name;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f); // degrees
    glm::vec3 scale    = glm::vec3(1.0f);

    std::shared_ptr<Node> rootNode;
    std::shared_ptr<Shader> shader;

    RenderType renderType = RenderType::Mesh;

public:
    Entity(
        const std::string& entityName,
        const std::shared_ptr<Node>& root,
        const std::shared_ptr<Shader>& shaderProgram
    )
        : name(entityName),
          rootNode(root),
          shader(shaderProgram)
    {
    }

    const std::string& getName() const {
        return name;
    }

    std::shared_ptr<Node> getRootNode() const {
        return rootNode;
    }

    std::shared_ptr<Shader> getShader() const {
        return shader;
    }

    void setPosition(const glm::vec3& pos) {
        position = pos;
    }

    void setRotation(const glm::vec3& rot) {
        rotation = rot;
    }

    void setScale(const glm::vec3& s) {
        scale = s;
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

    void setRenderType(RenderType type) {
        renderType = type;
    }

    RenderType getRenderType() const {
        return renderType;
}
};
