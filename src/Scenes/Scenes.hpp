#pragma once

#include <variant>
#include "Cord.hpp"
#include "Cloth.hpp"
#include "Spheres.hpp"
#include "SoftBody.hpp"

enum class SceneType {
    CORD,
    CLOTH,
    SPHERES,
    RIGID,
};

class Scenes {
public:
    static Scene *createScene(SceneType type, Scene *scene) {
        switch (type) {
        case SceneType::CORD:
            return new Cord(dynamic_cast<const Cord &>(*scene));

        case SceneType::CLOTH:
            return new Cloth(dynamic_cast<const Cloth &>(*scene));

        case SceneType::SPHERES:
            return new Spheres(dynamic_cast<const Spheres &>(*scene));

        case SceneType::RIGID:
            return new SoftBody(dynamic_cast<const SoftBody &>(*scene));

        default:
            std::cout << "Wrong scene type" << std::endl;
            return nullptr;
        }
    }

    static Scene *createScene(SceneType type) {
        switch (type) {
        case SceneType::CORD:
            return new Cord();

        case SceneType::CLOTH:
            return new Cloth();

        case SceneType::SPHERES:
            return new Spheres();

        case SceneType::RIGID:
            return new SoftBody();

        default:
            std::cout << "Wrong scene type" << std::endl;
            return nullptr;
        }
    }

    inline static const std::vector<const char *> sceneNames = {"Cord", "Cloth", "Spheres", "Soft Body"};
};
