#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "model.hpp"
#include "shader.hpp"
#include "gaussian_splat_model.hpp"

class Node : public std::enable_shared_from_this<Node> {
private:
    std::string name;

    glm::vec3 localPosition = glm::vec3(0.0f);
    glm::quat localRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 localScale    = glm::vec3(1.0f);

    bool hasJoint = false;
    bool isSlideJoint = false;
    glm::vec3 jointAxis = glm::vec3(0.0f);
    float jointValue = 0.0f;

    std::shared_ptr<Model> model = nullptr; 
    std::shared_ptr<GaussianSplatModel> gaussianModel = nullptr;

    std::vector<std::shared_ptr<Node>> children;

public:
    explicit Node(const std::string& nodeName = "unnamed")
        : name(nodeName)
    {
    }

    const std::string& getName() const {
        return name;
    }

    void setName(const std::string& nodeName) {
        name = nodeName;
    }

    void setModel(const std::shared_ptr<Model>& modelAsset) {
        model = modelAsset;
    }

    std::shared_ptr<Model> getModel() const {
        return model;
    }

    bool hasModel() const {
        return model != nullptr;
    }

    void setLocalPosition(const glm::vec3& position) {
        localPosition = position;
    }

    void setLocalRotation(const glm::quat& rotation) {
        localRotation = rotation;
    }

    void setLocalScale(const glm::vec3& scale) {
        localScale = scale;
    }

    const glm::vec3& getLocalPosition() const {
        return localPosition;
    }

    const glm::quat& getLocalRotation() const {
        return localRotation;
    }

    const glm::vec3& getLocalScale() const {
        return localScale;
    }

    void setJoint(
        const glm::vec3& axis,
        bool slideJoint = false,
        float value = 0.0f
    ) {
        hasJoint = true;
        isSlideJoint = slideJoint;
        jointAxis = axis;
        jointValue = value;
    }

    bool getHasJoint() const {
        return hasJoint;
    }

    bool getIsSlideJoint() const {
        return isSlideJoint;
    }

    const glm::vec3& getJointAxis() const {
        return jointAxis;
    }

    float getJointValue() const {
        return jointValue;
    }

    void setJointValue(float value) {
        jointValue = value;
    }

    void addChild(const std::shared_ptr<Node>& child) {
        children.push_back(child);
    }

    const std::vector<std::shared_ptr<Node>>& getChildren() const {
        return children;
    }

    std::vector<std::shared_ptr<Node>>& getChildren() {
        return children;
    }

    glm::mat4 getLocalMatrix() const {
        glm::mat4 local = glm::mat4(1.0f);

        local = glm::translate(local, localPosition);
        local *= glm::toMat4(localRotation);
        local = glm::scale(local, localScale);

        if (hasJoint) {
            if (isSlideJoint) {
                local = glm::translate(local, jointAxis * jointValue);
            } else {
                local = glm::rotate(
                    local,
                    jointValue,
                    glm::normalize(jointAxis)
                );
            }
        }

        return local;
    }

    std::shared_ptr<Node> findNode(const std::string& searchName) {
        if (name == searchName) {
            return shared_from_this();
        }

        for (const auto& child : children) {
            auto result = child->findNode(searchName);

            if (result) {
                return result;
            }
        }

        return nullptr;
    }

    void setGaussianModel(const std::shared_ptr<GaussianSplatModel>& splatModel) {
        gaussianModel = splatModel;
    }

    std::shared_ptr<GaussianSplatModel> getGaussianModel() const {
        return gaussianModel;
    }

    bool hasGaussianModel() const {
        return gaussianModel != nullptr;
    }

    void printTree(int depth = 0) const {
        std::string indent(depth * 2, ' ');

        std::cout << indent << "- " << name;

        if (model) {
            std::cout << " [model]";
        }

        if (hasJoint) {
            std::cout << " [joint]";
        }

        std::cout << '\n';

        for (const auto& child : children) {
            child->printTree(depth + 1);
        }
    }
};

