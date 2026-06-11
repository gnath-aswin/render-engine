
#include <glad/glad.h>

#include <iostream>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <cstddef>
#include "gaussian_splat_model.hpp"

// helpers
namespace {
float sigmoid(float x){
    return 1.0f / (1.0f + std::exp(-x));
}

float clamp01(float x){
    return std::max(0.0f, std::min(1.0f, x));
}

// Color encoded in Spherical harmonics: different colour when viewed from different angle 
float shToRgb(float dc){
    constexpr float SH_C0 = 0.28209479177387814f;
    return clamp01(0.5f + SH_C0 * dc);
}
}

GaussianSplatModel::GaussianSplatModel(const std::string& path, size_t maxGaussians, size_t stride)
{
  loadFromPLY(path, maxGaussians, stride);
  // uploadPointToGPU();
  uploadQuadToGPU();
  checkGLError("Gaussian uploadToGPU");
}

GaussianSplatModel::~GaussianSplatModel()
{
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
    }

    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
    }

    if (quadVBO != 0) {
        glDeleteBuffers(1, &quadVBO);
    }

    if (gaussianVBO != 0) {
        glDeleteVertexArrays(1, &gaussianVBO);
    }
}

void GaussianSplatModel::loadSamplePoints(const std::string& path)
{
    gaussians.clear();

    std::cout << "Loading gaussian scene: " << path << std::endl;

    // Temporary dummy point cloud.
    // Later replace this with real .ply 3DGS loading.
    std::mt19937 rng(42); //Mersenne Twister pseudo-random generator
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    std::uniform_real_distribution<float> colorDist(0.2f, 1.0f);

    for (int i = 0; i < 1000000; ++i) {
        Gaussian g;

        g.position = glm::vec3(
            dist(rng),
            dist(rng),
            dist(rng)
        );

        g.opacity = 0.8f;

        g.scale = glm::vec3(0.01f);
        g.padding = 0.0f;

        g.color = glm::vec4(
            colorDist(rng),
            colorDist(rng),
            colorDist(rng),
            1.0f
        );

        gaussians.push_back(g);
    }

    std::cout << "Loaded gaussians: " << gaussians.size() << std::endl;
}

void GaussianSplatModel::loadFromPLY(const std::string& path, size_t maxGaussiansToLoad, size_t stride)
{
    gaussians.clear();

    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Failed to open PLY file: " << path << std::endl;
        return;
    }

    std::string line;
    bool isBinaryLittleEndian = false;
    bool inVertexElement = false;

    size_t vertexCount = 0;
    std::vector<std::string> properties;

    // --------------------
    // Parse header as text
    // --------------------
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line == "format binary_little_endian 1.0") {
            isBinaryLittleEndian = true;
        }

        if (line.rfind("element vertex", 0) == 0) {
            std::stringstream ss(line);

            std::string elementWord;
            std::string vertexWord;

            ss >> elementWord >> vertexWord >> vertexCount;

            inVertexElement = true;
            continue;
        }

        if (line.rfind("element ", 0) == 0 &&
            line.rfind("element vertex", 0) != 0)
        {
            inVertexElement = false;
        }

        if (inVertexElement && line.rfind("property", 0) == 0) {
            std::stringstream ss(line);

            std::string propertyWord;
            std::string typeName;
            std::string propertyName;

            ss >> propertyWord >> typeName >> propertyName;

            // For your file, all are float.
            properties.push_back(propertyName);
            continue;
        }

        if (line == "end_header") {
            break;
        }
    }

    if (!isBinaryLittleEndian) {
        std::cout << "Only binary_little_endian PLY is supported now: "
                  << path << std::endl;
        return;
    }

    if (vertexCount == 0 || properties.empty()) {
        std::cout << "Invalid PLY header: no vertices/properties\n";
        return;
    }

    std::unordered_map<std::string, int> propertyIndex;

    for (int i = 0; i < static_cast<int>(properties.size()); ++i) {
        propertyIndex[properties[i]] = i;
    }

    auto getIndex = [&](const std::string& name) -> int {
        auto it = propertyIndex.find(name);

        if (it == propertyIndex.end()) {
            return -1;
        }

        return it->second;
    };

    int ix = getIndex("x");
    int iy = getIndex("y");
    int iz = getIndex("z");

    int ir0 = getIndex("rot_0");
    int ir1 = getIndex("rot_1");
    int ir2 = getIndex("rot_2");
    int ir3 = getIndex("rot_3");

    int is0 = getIndex("scale_0");
    int is1 = getIndex("scale_1");
    int is2 = getIndex("scale_2");

    int if0 = getIndex("f_dc_0");
    int if1 = getIndex("f_dc_1");
    int if2 = getIndex("f_dc_2");

    int iopacity = getIndex("opacity");

    if (ix < 0 || iy < 0 || iz < 0) {
        std::cout << "PLY missing x/y/z fields\n";
        return;
    }

    const size_t propertyCount = properties.size();

    std::cout << "PLY vertex count: " << vertexCount << std::endl;
    std::cout << "PLY property count: " << propertyCount << std::endl;

    size_t countToRead = std::min(vertexCount, maxGaussiansToLoad);

    gaussians.reserve(countToRead);
    // gaussians.reserve(vertexCount);

    std::vector<float> values(propertyCount);

    for (size_t i = 0; i < countToRead; ++i) {
        file.read(
            reinterpret_cast<char*>(values.data()),
            static_cast<std::streamsize>(propertyCount * sizeof(float))
        );

        if (!file) {
            std::cout << "Stopped early while reading binary PLY at vertex "
                      << i << std::endl;
            break;
        }

        if (i % stride != 0){
          continue;
        }

        Gaussian g;

        g.position = glm::vec3(
            values[ix],
            values[iy],
            values[iz]
        );

        g.rotation = glm::vec4(
            ir0 >= 0 ? values[ir0] : 1.0f,
            ir1 >= 0 ? values[ir1] : 0.0f,
            ir2 >= 0 ? values[ir2] : 0.0f,
            ir3 >= 0 ? values[ir3] : 0.0f
        );

        float fdc0 = if0 >= 0 ? values[if0] : 0.0f;
        float fdc1 = if1 >= 0 ? values[if1] : 0.0f;
        float fdc2 = if2 >= 0 ? values[if2] : 0.0f;

        g.color = glm::vec4(
            shToRgb(fdc0),
            shToRgb(fdc1),
            shToRgb(fdc2),
            1.0f
        );

        float rawOpacity = iopacity >= 0 ? values[iopacity] : 10.0f;
        g.opacity = sigmoid(rawOpacity);

        float rawS0 = is0 >= 0 ? values[is0] : -4.5f;
        float rawS1 = is1 >= 0 ? values[is1] : -4.5f;
        float rawS2 = is2 >= 0 ? values[is2] : -4.5f;

        // 3DGS stores scale in log-space.
        g.scale = glm::vec3(
            std::exp(rawS0),
            std::exp(rawS1),
            std::exp(rawS2)
        );

        g.padding = 0.0f;

        gaussians.push_back(g);
    }

    std::cout << "Loaded binary PLY gaussians: "
              << gaussians.size()
              << " from "
              << path
              << std::endl;

    for (size_t i = 0; i < std::min<size_t>(5, gaussians.size()); ++i) {
        const Gaussian& g = gaussians[i];

        std::cout << "Gaussian " << i
                  << " pos=("
                  << g.position.x << ", "
                  << g.position.y << ", "
                  << g.position.z << ") color=("
                  << g.color.r << ", "
                  << g.color.g << ", "
                  << g.color.b << ") opacity="
                  << g.opacity << " scale=("
                  << g.scale.x << ", "
                  << g.scale.y << ", "
                  << g.scale.z << ")"
                  << std::endl;
    }
}

void GaussianSplatModel::uploadPointToGPU()
{
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
    }

    if (VBO == 0) {
        glGenBuffers(1, &VBO);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(gaussians.size() * sizeof(Gaussian)),
        gaussians.data(),
        GL_DYNAMIC_DRAW
    );

    // position: vec3
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Gaussian),
        reinterpret_cast<void*>(offsetof(Gaussian, position))
    );

    // opacity: float
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Gaussian),
        reinterpret_cast<void*>(offsetof(Gaussian, opacity))
    );

    // scale: vec3
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Gaussian),
        reinterpret_cast<void*>(offsetof(Gaussian, scale))
    );

    // color: vec4
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3,
        4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Gaussian),
        reinterpret_cast<void*>(offsetof(Gaussian, color))
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GaussianSplatModel::uploadQuadToGPU()
{
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
    }

    if (quadVBO == 0) {
        glGenBuffers(1, &quadVBO);
    }

    if (gaussianVBO == 0) {
        glGenBuffers(1, &gaussianVBO);
    }

    float quadVertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    glBindVertexArray(VAO);
    checkGLError("VAO");

    // -------------------------------------------------
    // Quad corner VBO: per-vertex data
    // location 0 = aCorner
    // -------------------------------------------------
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(quadVertices),
        quadVertices,
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(float),
        reinterpret_cast<void*>(0)
    );

    // This changes every vertex, not every instance.
    glVertexAttribDivisor(0, 0);

    // -------------------------------------------------
    // Gaussian VBO: per-instance data
    // -------------------------------------------------
    glBindBuffer(GL_ARRAY_BUFFER, gaussianVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(gaussians.size() * sizeof(Gaussian)),
        gaussians.data(),
        GL_DYNAMIC_DRAW
    );

    // location 1 = aPos
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Gaussian),
        reinterpret_cast<void*>(offsetof(Gaussian, position))
    );
    glVertexAttribDivisor(1, 1);

    // location 2 = aOpacity
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Gaussian),
        reinterpret_cast<void*>(offsetof(Gaussian, opacity))
    );
    glVertexAttribDivisor(2, 1);

    // location 3 = aScale
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Gaussian),
        reinterpret_cast<void*>(offsetof(Gaussian, scale))
    );
    glVertexAttribDivisor(3, 1);

    // location 4 = aRotation
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(
        4,
        4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Gaussian),
        reinterpret_cast<void*>(offsetof(Gaussian, rotation))
    );
    glVertexAttribDivisor(4, 1);

    // location 5 = aColor
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(
        5,
        4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Gaussian),
        reinterpret_cast<void*>(offsetof(Gaussian, color))
    );
    glVertexAttribDivisor(5, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GaussianSplatModel::draw() const
{
  if (VAO == 0 || gaussians.empty()) {
        std::cout << "Gaussian draw skipped. VAO="
                  << VAO
                  << ", count="
                  << gaussians.size()
                  << std::endl;
        return;
    }

  glBindVertexArray(VAO);
  // Draw points
  // glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(gaussians.size()));

  // Draw quad
  glDrawArraysInstanced(
      GL_TRIANGLE_STRIP,
      0,
      4,
      static_cast<GLsizei>(gaussians.size())
  );

  glBindVertexArray(0);
}

void GaussianSplatModel::pruneByOpacity(float minOpacity)
{
    gaussians.erase(
        std::remove_if(
            gaussians.begin(),
            gaussians.end(),
            [minOpacity](const Gaussian& g) {
                return g.opacity < minOpacity;
            }
        ),
        gaussians.end()
    );

    uploadPointToGPU();

    std::cout << "After opacity pruning: "
              << gaussians.size()
              << " gaussians\n";
}

void GaussianSplatModel::pruneByScale(float maxScale)
{
    gaussians.erase(
        std::remove_if(
            gaussians.begin(),
            gaussians.end(),
            [maxScale](const Gaussian& g) {
                float s = glm::max(g.scale.x, glm::max(g.scale.y, g.scale.z));
                return s > maxScale;
            }
        ),
        gaussians.end()
    );

    uploadPointToGPU();

    std::cout << "After scale pruning: "
              << gaussians.size()
              << " gaussians\n";
}

void GaussianSplatModel::checkGLError(const std::string& label)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL error after " << label
                  << ": " << err << std::endl;
    }
}
