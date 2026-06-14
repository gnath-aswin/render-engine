#include <glm/gtx/io.hpp>
#include <math.h>
#include <memory>

#include "entity.hpp"
#include "gaussian_background_renderer.hpp"
#include "gaussian_splat_model.hpp"
#include "mujoco_robot_loader.hpp"
#include "shader.hpp"
#include "window.hpp"

int main() {
  // Initialize window first (creates the OpenGL context)
  Window window(1980, 1080, "Robot in Mars");
  if (!window.init())
    return -1;

  // Shader
  // --------
  // Robots
  char vertexShaderSource[] = "../shaders/trlc_robot.vs";
  char fragmentShaderSource[] = "../shaders/trlc_robot.fs";
  auto robotShader =
      std::make_shared<Shader>(vertexShaderSource, fragmentShaderSource);

  // Gaussian splat1
  char gsVertexShaderSource[] = "../shaders/gaussian/gaussian_covariance.vs";
  char gsFragmentShaderSource[] = "../shaders/gaussian/gaussian_covariance.fs";
  auto gaussianQuadsShader =
      std::make_shared<Shader>(gsVertexShaderSource, gsFragmentShaderSource);

  // Gaussian Splat points
  char gpVertexShaderSource[] = "../shaders/gaussian/gaussian.vs";
  char gpFragmentShaderSource[] = "../shaders/gaussian/gaussian.fs";
  auto gaussianPointsShader =
      std::make_shared<Shader>(gpVertexShaderSource, gpFragmentShaderSource);

  // Gaussian Splat points
  char gqVertexShaderSource[] = "../shaders/gaussian/gaussian_covariance.vs";
  char gqFragmentShaderSource[] = "../shaders/gaussian/gaussian_covariance.fs";
  auto gaussianCovShader =
      std::make_shared<Shader>(gqVertexShaderSource, gqFragmentShaderSource);
  // set shader in windows for render mode

  // Entities
  // -----------
  // Robot
  MuJoCoRobotLoader loader;
  auto robotRoot = loader.load("../resources/humanoid/g1.xml");
  Entity robotEntity("robot", robotRoot, robotShader);
  robotEntity.setRenderType(RenderType::Mesh);
  robotEntity.setRotation(glm::vec3(-90, 0, 0));
  int depth = 0;
  robotRoot->printTree();

  // Gaussian Model
  auto gaussianModel = std::make_shared<GaussianSplatModel>("../resources/gaussian/scene.ply", 6000000, 10, false);
  auto gaussianBackground = std::make_shared<GaussianBackgroundRenderer>(gaussianModel, gaussianPointsShader, gaussianQuadsShader, gaussianCovShader);
  gaussianBackground->setRotation(glm::vec3(-180, 0, 0));
  gaussianBackground->setPosition(glm::vec3(-5, 0, 0));
  gaussianBackground->setScale(glm::vec3(1.0f));

  window.setGaussianRenderer(gaussianBackground);

  // Scene
  // ---------
  Scene scene;
  scene.addEntity(robotEntity);
  window.renderLoop(scene);

  return 0;
}
