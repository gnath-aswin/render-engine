#include "mujoco_robot_loader.hpp"

#include <tinyxml2.h>
#include <iostream>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

glm::vec3 MuJoCoRobotLoader::parseVec3(const char* text)
{
    if (!text) {
        return glm::vec3(0.0f);
    }

    std::stringstream ss(text);

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    ss >> x >> y >> z;

    return glm::vec3(x, y, z);
}

glm::quat MuJoCoRobotLoader::parseQuat(const char* text)
{
    if (!text) {
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    std::stringstream ss(text);

    float w = 1.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    ss >> w >> x >> y >> z;

    // MuJoCo quat order is w x y z.
    // GLM constructor is also w x y z.
    return glm::quat(w, x, y, z);
}

std::shared_ptr<Node> MuJoCoRobotLoader::load(const std::string& xmlPath)
{
    tinyxml2::XMLDocument doc;

    if (doc.LoadFile(xmlPath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cout << "Failed to load MuJoCo XML: " << xmlPath << std::endl;
        return nullptr;
    }

    fs::path xmlFsPath(xmlPath);
    xmlDirectory = xmlFsPath.parent_path().string();

    tinyxml2::XMLElement* root = doc.FirstChildElement("mujoco");

    if (!root) {
        std::cout << "No <mujoco> root element found\n";
        return nullptr;
    }

    tinyxml2::XMLElement* compiler = root->FirstChildElement("compiler");

    if (compiler && compiler->Attribute("meshdir")) {
        meshDirectory = (fs::path(xmlDirectory) / compiler->Attribute("meshdir"))
                            .lexically_normal()
                            .string();
    } else {
        meshDirectory = xmlDirectory;
    }

    tinyxml2::XMLElement* asset = root->FirstChildElement("asset");

    if (asset) {
        loadAssets(asset);
    }

    tinyxml2::XMLElement* worldbody = root->FirstChildElement("worldbody");

    if (!worldbody) {
        std::cout << "No <worldbody> found\n";
        return nullptr;
    }

    auto rootNode = std::make_shared<Node>("world");

    // Geoms directly under worldbody.
    for (tinyxml2::XMLElement* geom = worldbody->FirstChildElement("geom");
         geom;
         geom = geom->NextSiblingElement("geom"))
    {
        auto geomNode = createGeomNode(geom);

        if (geomNode) {
            rootNode->addChild(geomNode);
        }
    }

    // Nested body tree.
    for (tinyxml2::XMLElement* body = worldbody->FirstChildElement("body");
         body;
         body = body->NextSiblingElement("body"))
    {
        rootNode->addChild(processBody(body));
    }

    return rootNode;
}

void MuJoCoRobotLoader::loadAssets(tinyxml2::XMLElement* assetElement)
{
    meshFiles.clear();

    for (tinyxml2::XMLElement* mesh = assetElement->FirstChildElement("mesh");
         mesh;
         mesh = mesh->NextSiblingElement("mesh"))
    {
        const char* name = mesh->Attribute("name");
        const char* file = mesh->Attribute("file");

        if (!name || !file) {
            continue;
        }

        std::string fullPath = (fs::path(meshDirectory) / file)
                                   .lexically_normal()
                                   .string();

        meshFiles[name] = fullPath;

        std::cout << "Mesh asset: " << name
                  << " -> " << fullPath << std::endl;
    }
}

std::shared_ptr<Node> MuJoCoRobotLoader::createGeomNode(
    tinyxml2::XMLElement* geomElement
) {
    const char* meshName = geomElement->Attribute("mesh");

    if (!meshName) {
        return nullptr;
    }

    if (!meshFiles.count(meshName)) {
        std::cout << "Missing mesh asset: " << meshName << std::endl;
        return nullptr;
    }

    const char* geomName = geomElement->Attribute("name");

    auto geomNode = std::make_shared<Node>(
        geomName ? geomName : meshName
    );

    geomNode->setLocalPosition(parseVec3(geomElement->Attribute("pos")));
    geomNode->setLocalRotation(parseQuat(geomElement->Attribute("quat")));

    auto model = std::make_shared<Model>(meshFiles[meshName]);
    geomNode->setModel(model);

    std::cout << "Loaded geom mesh: " << meshName << std::endl;

    return geomNode;
}

std::shared_ptr<Node> MuJoCoRobotLoader::processBody(
    tinyxml2::XMLElement* bodyElement
) {
    auto node = std::make_shared<Node>(
        bodyElement->Attribute("name")
            ? bodyElement->Attribute("name")
            : "unnamed_body"
    );

    node->setLocalPosition(parseVec3(bodyElement->Attribute("pos")));
    node->setLocalRotation(parseQuat(bodyElement->Attribute("quat")));

    tinyxml2::XMLElement* joint = bodyElement->FirstChildElement("joint");

    if (joint) {
        glm::vec3 axis = parseVec3(joint->Attribute("axis"));

        bool isSlide = false;

        const char* type = joint->Attribute("type");

        if (type && std::string(type) == "slide") {
            isSlide = true;
        }

        node->setJoint(axis, isSlide);
    }

    // Every geom becomes a child node with one model.
    for (tinyxml2::XMLElement* geom = bodyElement->FirstChildElement("geom");
         geom;
         geom = geom->NextSiblingElement("geom"))
    {
        auto geomNode = createGeomNode(geom);

        if (geomNode) {
            node->addChild(geomNode);
        }
    }

    // Child bodies remain body nodes.
    for (tinyxml2::XMLElement* childBody = bodyElement->FirstChildElement("body");
         childBody;
         childBody = childBody->NextSiblingElement("body"))
    {
        node->addChild(processBody(childBody));
    }

    return node;
}
