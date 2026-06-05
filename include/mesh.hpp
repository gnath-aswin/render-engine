#pragma once
#include <vector>


struct VertexAttribute {
    unsigned int location; // layout location in shader
    int size;              // number of floats: vec2 = 2, vec3 = 3, vec4 = 4
    unsigned int offset;   // offset in floats
};

class Mesh {
  private:
      unsigned int VAO;
      unsigned int VBO;
      unsigned int EBO;
      unsigned int texture;
      int vertexCount;
      int indexCount;

  public:
      Mesh(const std::vector<float>& vertices, 
          const std::vector<unsigned int>& indices,
          int floatsPerVertex,
          const std::vector<VertexAttribute>& attributes);
      ~Mesh();

      void draw() const;
};

