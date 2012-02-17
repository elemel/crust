#ifndef CRUST_CHAIN_SCENE_COMPONENT_HPP
#define CRUST_CHAIN_SCENE_COMPONENT_HPP

#include "scene_component.hpp"

namespace crust {
    class Actor;
    class ChainPhysicsComponent;

    class ChainSceneComponent : public SceneComponent {
    public:
        explicit ChainSceneComponent(Actor *actor);

        void create()
        { }

        void destroy()
        { }

        void draw() const;

    private:
        Actor *actor_;
        ChainPhysicsComponent *physicsComponent_;
    };
}

#endif
