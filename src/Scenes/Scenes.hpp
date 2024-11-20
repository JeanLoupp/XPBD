#pragma once

#include <variant>
#include "Cord.hpp"
#include "Cloth.hpp"
#include "RigidBody.hpp"

enum class SceneType {
    CORD,
    CLOTH,
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

        case SceneType::RIGID:
            return new RigidBody(dynamic_cast<const RigidBody &>(*scene));

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

        case SceneType::RIGID:
            return new RigidBody();

        default:
            std::cout << "Wrong scene type" << std::endl;
            return nullptr;
        }
    }

    inline static const std::vector<const char *> sceneNames = {"Cord", "Cloth", "Rigid Body"};
};
