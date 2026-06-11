#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <memory>

#include "window.hpp"
#include "scene.hpp"
#include "entity.hpp"
#include "node.hpp"

namespace {
void checkGLError(const std::string& label)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL error after " << label
                  << ": " << err << std::endl;
    }
}
}

Window::Window(int w, int h, const char *title)
    : window(nullptr), width(w), height(h), title(title),
      camera(glm::vec3(0.0f, 0.0f, 3.0f)),
      lastX(w / 2.0f),
      lastY(h / 2.0f),
      firstMouse(true),
      isLeftButtonPressed(false),
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
  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  return true;
}

void Window::drawNode(
    const std::shared_ptr<Node>& node,
    const glm::mat4& parentTransform,
    const std::shared_ptr<Shader>& shader
) {
    if (!node) {
        return;
    }

    glm::mat4 globalTransform = parentTransform * node->getLocalMatrix();

    if (node->hasModel()) {
        auto model = node->getModel();

        for (const auto& mesh : model->meshes) {
            bool hasDiffuse = false;

            unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;

            for (unsigned int j = 0; j < mesh->textures.size(); ++j) {
                mesh->textures[j]->bind(j);

                std::string name = mesh->textures[j]->getType();
                std::string number;

                if (name == "texture_diffuse") {
                    number = std::to_string(diffuseNr++);
                    hasDiffuse = true;
                }
                else if (name == "texture_specular") {
                    number = std::to_string(specularNr++);
                }

                shader->setInt(name + number, j);
            }

            shader->setBool("hasDiffuseTexture", hasDiffuse);
            shader->setMat4("model", globalTransform, node->getName());

            mesh->draw();
        }
    }

    if (node->hasGaussianModel()) {
        shader->setMat4("model", globalTransform, node->getName());
        shader->setFloat("splatSizeMultiplier", 100.0f);
        shader->setFloat("splatExtent", 3.0f);
        shader->setFloat("opacityScale", 0.5f);
        shader->setFloat("maxScreenRadius", 0.04f);
        node->getGaussianModel()->draw();
        checkGLError("Gaussian Draw");
    }

    for (const auto& child : node->getChildren()) {
        drawNode(child, globalTransform, shader);
    }
}

void Window::drawEntity(
    const Entity& entity,
    const glm::mat4& view,
    const glm::mat4& projection
) {
    auto shader = entity.getShader();

    shader->use();

    shader->setMat4("view", view, entity.getName());
    shader->setMat4("projection", projection, entity.getName());

    shader->setVec3("baseColor", 0.3f, 0.8f, 0.8f);
    shader->setVec3("lightPos", 3.0f, 5.0f, 3.0f);
    shader->setVec3(
        "viewPos",
        camera.Position.x,
        camera.Position.y,
        camera.Position.z
    );

    glm::mat4 entityTransform = entity.getModelMatrix();

    drawNode(
        entity.getRootNode(),
        entityTransform,
        shader
    );
}

// render loop
// -----------
void Window::renderLoop(Scene& scene)
{
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom),
            static_cast<float>(width) / static_cast<float>(height),
            0.1f,
            100.0f
        );

        glm::mat4 view = camera.GetViewMatrix();

        for (const Entity& entity : scene.getEntities()) {
            drawEntity(entity, view, projection);
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

  // Show cursor when its hidden
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
  // Look around with mouse left button
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) isLeftButtonPressed = true;
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
    isLeftButtonPressed = false;
    firstMouse = true;
  }
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

    if(self->isLeftButtonPressed){
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
