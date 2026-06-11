#include <glad/glad.h>
#include "mesh.hpp"
#include "texture.hpp"
#include <memory>


Mesh::Mesh(const std::vector<Vertex>& vertices,
          const std::vector<unsigned int>& indices,
          const std::vector<std::shared_ptr<Texture>>& textures):
          vertices(vertices),
          indices(indices),
          textures(textures)
{
    indexCount = indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),//********** i have doubt here *******************// 
        vertices.data(),
        GL_STATIC_DRAW
    );
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size()*sizeof(unsigned int),
        indices.data(), 
        GL_STATIC_DRAW
    );

 
    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// ids
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

		// weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::draw() const {
  // std::cout << "Drawing mesh with " << indexCount << " indices\n";
  // Draw mode: GL_Line or GL_FILL
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindVertexArray(VAO);
  // glDrawArrays(GL_TRIANGLES, 0, vertexCount);
  glDrawElements(GL_TRIANGLES, indexCount,GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
