#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "texture.hpp"

#define MAX_BONE_INFLUENCE 4
struct Vertex {
  // position
  glm::vec3 Position;
  // normal
  glm::vec3 Normal;
  // texCoords
  glm::vec2 TexCoords;
  // tangent
  glm::vec3 Tangent;
  // bitangent
  glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

class Mesh {
  private:
      unsigned int VAO;
      unsigned int VBO;
      unsigned int texture;
      unsigned int EBO;
      int vertexCount;
      int indexCount;

  public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::shared_ptr<Texture>> textures;

    Mesh(const std::vector<Vertex>& vertices, 
        const std::vector<unsigned int>& indices,
        const std::vector<std::shared_ptr<Texture>>& textures);
    ~Mesh();
 
    // No copy instructions
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw() const;
};

