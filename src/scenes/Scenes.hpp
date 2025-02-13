// Stores all the scenes available and allows them to be visible in the interface.

#pragma once

#include <variant>
#include "Cord.hpp"
#include "Cloth.hpp"
#include "ClothDrop.hpp"
#include "Spheres.hpp"
#include "SoftBody.hpp"
#include "SoftBall.hpp"
#include "RigidBody.hpp"
#include "Fluid.hpp"

enum class SceneType {
    CORD,
    CLOTH,
    CLOTHDROP,
    SPHERES,
    SOFTBODY,
    SOFTBALL,
    RIGIDBODY,
    FLUID
};

class Scenes {
public:
    static Scene *createScene(SceneType type, Scene *scene) {
        switch (type) {
        case SceneType::CORD:
            return new Cord(dynamic_cast<const Cord &>(*scene));

        case SceneType::CLOTH:
            return new Cloth(dynamic_cast<const Cloth &>(*scene));

        case SceneType::CLOTHDROP:
            return new ClothDrop(dynamic_cast<const ClothDrop &>(*scene));

        case SceneType::SPHERES:
            return new Spheres(dynamic_cast<const Spheres &>(*scene));

        case SceneType::SOFTBODY:
            return new SoftBody(dynamic_cast<const SoftBody &>(*scene));

        case SceneType::SOFTBALL:
            return new SoftBall(dynamic_cast<const SoftBall &>(*scene));

        case SceneType::RIGIDBODY:
            return new RigidBody(dynamic_cast<const RigidBody &>(*scene));

        case SceneType::FLUID:
            return new Fluid(dynamic_cast<const Fluid &>(*scene));

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

        case SceneType::CLOTHDROP:
            return new ClothDrop();

        case SceneType::SPHERES:
            return new Spheres();

        case SceneType::SOFTBODY:
            return new SoftBody();

        case SceneType::SOFTBALL:
            return new SoftBall();

        case SceneType::RIGIDBODY:
            return new RigidBody();

        case SceneType::FLUID:
            return new Fluid();

        default:
            std::cout << "Wrong scene type" << std::endl;
            return nullptr;
        }
    }

    inline static const std::vector<const char *> sceneNames = {"Cord", "Cloth", "Cloth Drop", "Spheres",
                                                                "Soft Body", "Soft Ball", "Rigid Body", "Fluid"};
};
