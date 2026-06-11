#pragma once

#include <vector>
#include "entity.hpp"

class Scene {
private:
    std::vector<Entity> entities;

public:
    void addEntity(const Entity& entity) {
        entities.push_back(entity);
    }

    std::vector<Entity>& getEntities() {
        return entities;
    }

    const std::vector<Entity>& getEntities() const {
        return entities;
    }
};
