#include <glad/glad.h>
#include "mesh.hpp"


Mesh::Mesh(const std::vector<float>& vertices,
          const std::vector<unsigned int>& indices,
          const int floatsPerVertex,
          const std::vector<VertexAttribute>& attributes)
{
    vertexCount = vertices.size() / floatsPerVertex;
    indexCount = indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size()*sizeof(unsigned int),
        indices.data(), 
        GL_STATIC_DRAW
    );

 
    for(const VertexAttribute& attribute: attributes){
      glVertexAttribPointer(
          attribute.location,
          attribute.size,
          GL_FLOAT,
          GL_FALSE,
          floatsPerVertex * sizeof(float),
          (void*)(attribute.offset * sizeof(float))
      );
      glEnableVertexAttribArray(attribute.location);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::draw() const {
  // Draw mode: GL_Line or GL_FILL
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, vertexCount);
  // glDrawElements(GL_TRIANGLES, indexCount,GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
