#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "gaussian_splat_model.hpp"

class GaussianBackgroundRenderer {
private:
  std::shared_ptr<GaussianSplatModel> model;

  std::shared_ptr<Shader> pointShader;
  std::shared_ptr<Shader> quadShader;
  std::shared_ptr<Shader> covarianceShader;

  GaussianRenderMode mode = GaussianRenderMode::Points;

  bool requestSort = false;

  float pointSizeMultiplier = 800.0f;
  float splatSizeMultiplier = 100.0f;
  float splatExtent = 3.0f;
  float opacityScale = 0.5f;
  float maxScreenRadius = 0.04f;
  
  // Transformation
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 rotation = glm::vec3(0.0f); // degrees
  glm::vec3 scale = glm::vec3(1.0f);

public:
  GaussianBackgroundRenderer(
      const std::shared_ptr<GaussianSplatModel>& gaussianModel,
      const std::shared_ptr<Shader>& point,
      const std::shared_ptr<Shader>& quad,
      const std::shared_ptr<Shader>& covariance
  );

  void setMode(GaussianRenderMode newMode);
  GaussianRenderMode getMode() const;
  
  void setPosition(const glm::vec3& p) { position = p; }
  void setRotation(const glm::vec3& r) { rotation = r; }
  void setScale(const glm::vec3& s) { scale = s; }
  
  std::shared_ptr<GaussianSplatModel> getGaussianModel() const{
    return model;
  }

  glm::mat4 getModelMatrix() const;

  void requestDepthSort();

  void render(
      const glm::mat4& view,
      const glm::mat4& projection
  );
};
