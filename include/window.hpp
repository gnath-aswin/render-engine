#pragma once
#include "camera.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

#include "scene.hpp"
#include "node.hpp"

class Window {
private:
  GLFWwindow *window;
  int width;
  int height;
  const char *title;
  // Camera properties
  Camera camera;
  float lastX;
  float lastY;
  bool isLeftButtonPressed;
  bool firstMouse;

  // timing
  float deltaTime;// time between current frame and last frame
  float lastFrame;
  float mixValue;

public:
  Window(int w, int h, const char *title);
  ~Window();

  bool init();
  void renderLoop(Scene& scene);

private:
  void processInput(GLFWwindow *window);
  static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
  static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
  static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
  void drawEntity(
        const Entity& entity,
        const glm::mat4& view,
        const glm::mat4& projection
    );
  void drawNode(
        const std::shared_ptr<Node>& node,
        const glm::mat4& parentTransform,
        const std::shared_ptr<Shader>& shader
    );
};
