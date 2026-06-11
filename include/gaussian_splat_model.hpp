#pragma once

#include "glm/glm.hpp"
#include <vector>
#include <string>

// With added padding for 64 bytes chunk
struct Gaussian{
  glm::vec3 position; // x, y , z
  float opacity;
  glm::vec3 scale; // 
  float padding;
  glm::vec4 rotation; // rot_0, rot_1, rot_2, rot_3
  glm::vec4 color;
};

class GaussianSplatModel{
  private:
    std::vector<Gaussian> gaussians;

    unsigned VAO = 0;
    unsigned VBO = 0;
    unsigned quadVBO = 0;
    unsigned gaussianVBO = 0;


  public:
    GaussianSplatModel() = default; // Empty constructor
    explicit GaussianSplatModel(const std::string& path, size_t maxGaussians, size_t stride);

    ~GaussianSplatModel();

    // Delete copying and copy assignment
    GaussianSplatModel(const GaussianSplatModel&) = delete;
    GaussianSplatModel& operator=(const GaussianSplatModel&) = delete;
    
    // Load splats
    void loadFromPLY(const std::string& path, size_t maxGaussians, size_t stride);
    void loadSamplePoints(const std::string& path);
    void uploadPointToGPU();
    void uploadQuadToGPU();
    void draw() const;

    // Reduce size
    void pruneByOpacity(float minOpacity);
    void pruneByScale(float maxScale);
    
    size_t size() const{
      return gaussians.size();
    }
    void checkGLError(const std::string& label);
};
