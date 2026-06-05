#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <glm/glm.hpp>

#include "window.hpp"
#include "entity.hpp"

Window::Window(int w, int h, const char *title)
    : window(nullptr), width(w), height(h), title(title),
      camera(glm::vec3(0.0f, 0.0f, 3.0f)),
      lastX(w / 2.0f),
      lastY(h / 2.0f),
      firstMouse(true),
      deltaTime(0.0f),
      lastFrame(0.0f),
      mixValue(0.2){}

Window::~Window() { glfwTerminate(); }

bool Window::init() {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // glfw window creation
  // --------------------
  window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create window!" << std::endl;
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to load GLAD" << std::endl;
    return false;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  return true;
}

// render loop
// -----------
void Window::renderLoop(std::vector<Entity>& objects) {
  while (!glfwWindowShouldClose(window)) {
    // per-frame time logic
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Input
    processInput(window);

    // Render
    // ------
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Projection matrix(View to clip) 
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(camera.Zoom), (float)width/(float)height, 0.1f, 100.0f);

    // camera/view transformation
    glm::mat4 view = camera.GetViewMatrix();

    // Render objects
    for (unsigned int i=0; i < objects.size(); ++i) {
      // Render data of each object: mesh, shader, texture
      const RenderObject& renderData = objects[i].getRenderData();
      // Use corresponding shader
      renderData.shader->use();
      // Setting shader uniform values: texture1, texture2, mix_ratio for each texture
      for (unsigned int j = 0; j < renderData.textures.size(); ++j) {
        renderData.textures[j]->bind(j);
        std::string uniformName = "texture" + std::to_string(j + 1);
        renderData.shader->setInt(uniformName, j);
      }
      renderData.shader->setFloat("mix_ratio", mixValue);

      // Transformations
      // Modify model matrix
      if(i % 2 == 0){objects[i].rotate(glm::vec3(5.0 * glfwGetTime(), 0.0, 10.0 * glfwGetTime()));}
      else {objects[i].rotate(glm::vec3(5 * glfwGetTime(), 10 * glfwGetTime(), 0.0f));}
      // Set model matrix: translation and rotation of individual object 
      renderData.shader->setMat4("model", objects[i].getModelMatrix());
      // Set camera/view matrix World to view(camera) 
      renderData.shader->setMat4("view", view);
      // Projection (View to clip)
      renderData.shader->setMat4("projection", projection);

      // Draw
      renderData.mesh->draw();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Window::processInput(GLFWwindow *window) {
  // Escape
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  // Mixing texture
  if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
    mixValue += 0.01f;
    if(mixValue > 1.0f){mixValue = 1.0f;}
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
    mixValue -= 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
    if (mixValue <= 0.0f) mixValue = 0.0f;
  }

  // Move camera around
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void Window::framebuffer_size_callback(GLFWwindow* glfwWindow, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);

    Window* self = static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
    if (!self) {
            return;
        }
    self -> width = width;
    self -> height = height;
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Window::mouse_callback(GLFWwindow* glfwWindow, double xposIn, double yposIn)
{
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

    if (!self) {
        return;
    }

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (self->firstMouse) {
        self->lastX = xpos;
        self->lastY = ypos;
        self->firstMouse = false;
    }

    float xoffset = xpos - self->lastX;
    float yoffset = self->lastY - ypos;

    self->lastX = xpos;
    self->lastY = ypos;

    self->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Window::scroll_callback(GLFWwindow* glfwWindow, double xoffset, double yoffset)
{
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

    if (!self) {
        return;
    }

    self->camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
