#include "gaussian_background_renderer.hpp"

#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

GaussianBackgroundRenderer::GaussianBackgroundRenderer(
    const std::shared_ptr<GaussianSplatModel>& gaussianModel,
    const std::shared_ptr<Shader>& point,
    const std::shared_ptr<Shader>& quad,
    const std::shared_ptr<Shader>& covariance
)
    : model(gaussianModel),
      pointShader(point),
      quadShader(quad),
      covarianceShader(covariance)
{
}

void GaussianBackgroundRenderer::setMode(GaussianRenderMode newMode)
{
    mode = newMode;

    if (model) {
        model->setRenderMode(mode);
    }

    switch (mode) {
        case GaussianRenderMode::Points:
            std::cout << "Gaussian mode: Points\n";
            break;
        case GaussianRenderMode::Quads:
            std::cout << "Gaussian mode: Quads\n";
            break;
        case GaussianRenderMode::CovarianceEllipses:
            std::cout << "Gaussian mode: CovarianceEllipses\n";
            break;
    }
}

GaussianRenderMode GaussianBackgroundRenderer::getMode() const
{
    return mode;
}

void GaussianBackgroundRenderer::requestDepthSort()
{
    requestSort = true;
}

glm::mat4 GaussianBackgroundRenderer::getModelMatrix() const
{
    glm::mat4 model(1.0f);

    model = glm::translate(model, position);

    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

    model = glm::scale(model, scale);

    return model;
}


void GaussianBackgroundRenderer::render(
    const glm::mat4& view,
    const glm::mat4& projection
) {
    if (!model) {
      std::cout << "No Gaussian Model found!" << std::endl;
      return;
    }

    model->setRenderMode(mode);

    std::shared_ptr<Shader> shader;

    switch (mode) {
        case GaussianRenderMode::Points:
            shader = pointShader;
            break;
        case GaussianRenderMode::Quads:
            shader = quadShader;
            break;
        case GaussianRenderMode::CovarianceEllipses:
        default:
            shader = covarianceShader;
            break;
    }

    if (!shader) {
        std::cout << "GaussianBackgroundRenderer: missing shader\n";
        return;
    }

    glm::mat4 modelMatrix = getModelMatrix();

    if (requestSort) {
        model->sortByDepth(modelMatrix, view);
        requestSort = false;
    }

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // For approximate background rendering, keep depth off.
    // Later we can write/read depth if needed.
    glDisable(GL_DEPTH_TEST);

    if (mode == GaussianRenderMode::Points) {
        glEnable(GL_PROGRAM_POINT_SIZE);
    }

    shader->use();
    shader->setMat4("model", modelMatrix, "gaussian_background");
    shader->setMat4("view", view, "gaussian_background");
    shader->setMat4("projection", projection, "gaussian_background");

    if (mode == GaussianRenderMode::Points) {
        shader->setFloat("pointSizeMultiplier", pointSizeMultiplier);
    }
    else if (mode == GaussianRenderMode::Quads) {
        shader->setFloat("splatSizeMultiplier", splatSizeMultiplier);
        shader->setFloat("opacityScale", opacityScale);
        shader->setFloat("maxScreenRadius", maxScreenRadius);
    }
    else if (mode == GaussianRenderMode::CovarianceEllipses) {
        shader->setFloat("splatExtent", splatExtent);
        shader->setFloat("opacityScale", opacityScale);
        shader->setFloat("maxScreenRadius", maxScreenRadius);
    }

    model->draw();

    glBindVertexArray(0);

    // Restore default for mesh pass.
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
