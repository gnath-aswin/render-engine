#pragma once

#include "glm/glm.hpp"
#include <future>
#include <string>
#include <vector>

// With added padding for 64 bytes chunk
struct Gaussian {
  glm::vec3 position; // x, y , z
  float opacity;
  glm::vec3 scale; //
  float padding;
  glm::vec4 rotation; // rot_0, rot_1, rot_2, rot_3
  glm::vec4 color;
};

// Enum for reder modes
enum class GaussianRenderMode { Points, Quads, CovarianceEllipses };

class GaussianSplatModel {
private:
  std::vector<Gaussian> gaussians;

  unsigned pointVAO = 0;
  unsigned pointVBO = 0;

  unsigned quadVAO = 0;
  unsigned quadVBO = 0;
  unsigned gaussianVBO = 0;

  GaussianRenderMode renderMode = GaussianRenderMode::Points;
  
  // 
  std::string path;
  size_t maxGaussians;
  size_t stride; 
  bool loadAllPoints;

public:
  GaussianSplatModel(
      const std::string& path,
      size_t maxGaussians = 100000,
      size_t stride = 10,
      bool loadAllPoints = false
  );


  ~GaussianSplatModel();

  // Delete copying and copy assignment
  GaussianSplatModel(const GaussianSplatModel &) = delete;
  GaussianSplatModel &operator=(const GaussianSplatModel &) = delete;

  // Load splats
  void loadFromPLY(const std::string &path, size_t maxGaussians,
                   size_t stride, bool loadAllPoints);
  void loadSamplePoints(const std::string &path);

  void uploadPointToGPU();
  void uploadQuadToGPU();
  void updateGaussianBuffer();

  void sortByDepth(const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix);
  
  void reloadAllGaussians();

  void setRenderMode(GaussianRenderMode mode);
  GaussianRenderMode getRenderMode() const;
  void draw() const;

  // Reduce size
  void pruneByOpacity(float minOpacity);
  void pruneByScale(float maxScale);

  size_t size() const { return gaussians.size(); }

  void checkGLError(const std::string &label);
};
