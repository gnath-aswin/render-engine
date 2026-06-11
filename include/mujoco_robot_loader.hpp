#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <tinyxml2.h>

#include "node.hpp"

class MuJoCoRobotLoader {
public:
    std::shared_ptr<Node> load(const std::string& xmlPath);

private:
    std::string xmlDirectory;
    std::string meshDirectory;

    std::unordered_map<std::string, std::string> meshFiles;

    void loadAssets(tinyxml2::XMLElement* assetElement);

    std::shared_ptr<Node> processBody(tinyxml2::XMLElement* bodyElement);
    std::shared_ptr<Node> createGeomNode(tinyxml2::XMLElement* geomElement);

    glm::vec3 parseVec3(const char* text);
    glm::quat parseQuat(const char* text);
};
