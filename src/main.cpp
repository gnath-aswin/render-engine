#include <math.h>
#include <memory>
#include <glm/gtx/io.hpp>

#include "entity.hpp"
#include "gaussian_splat_model.hpp"
#include "shader.hpp"
#include "window.hpp"
#include "mujoco_robot_loader.hpp"



int main(){
  // Initialize window first (creates the OpenGL context)
  Window window(1980, 1080, "Backpack");
  if (!window.init()) return -1;


  // Shader
  // --------
  // Robots
  char vertexShaderSource[] = "../shaders/trlc_robot.vs";
  char fragmentShaderSource[] = "../shaders/trlc_robot.fs";
  auto robotShader= std::make_shared<Shader>(vertexShaderSource, fragmentShaderSource);
  // Gaussian splat
  char gsVertexShaderSource[] = "../shaders/gaussian/gaussian_covariance.vs";
  char gsFragmentShaderSource[] = "../shaders/gaussian/gaussian_covariance.fs";
  auto gaussianShader= std::make_shared<Shader>(gsVertexShaderSource, gsFragmentShaderSource);

  
  // Entities
  // -----------
  // Robot
  MuJoCoRobotLoader loader;
  auto robotRoot = loader.load("../resources/humanoid/g1.xml");
  Entity robotEntity("robot", robotRoot, robotShader);
  int depth = 0;
  robotRoot->printTree();

  // Gaussian Splat
  auto gaussianModel = std::make_shared<GaussianSplatModel>(
      "../resources/gaussian/scene.ply",
      600000, 
      1
  );
  auto gaussianRoot = std::make_shared<Node>("gaussian_scene");
  gaussianRoot->setGaussianModel(gaussianModel);
  Entity gaussianEntity(
      "gaussian_scene",
      gaussianRoot,
      gaussianShader
  );
  gaussianEntity.setRotation(glm::vec3(-90, 0, 0));
  gaussianEntity.setPosition(glm::vec3(0.0f));
  gaussianEntity.setScale(glm::vec3(1.0f));


  // Scene
  Scene scene;
  scene.addEntity(robotEntity);
  scene.addEntity(gaussianEntity);
  window.renderLoop(scene);

  return 0;
}
