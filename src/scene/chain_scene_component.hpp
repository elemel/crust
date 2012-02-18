#ifndef CRUST_CHAIN_SCENE_COMPONENT_HPP
#define CRUST_CHAIN_SCENE_COMPONENT_HPP

#include "component.hpp"

namespace crust {
    class Actor;
    class ChainPhysicsComponent;

    class ChainSceneComponent : public Component {
    public:
        explicit ChainSceneComponent(Actor *actor);

        void create()
        { }

        void destroy()
        { }
        
        void draw();

    private:
        Actor *actor_;
        ChainPhysicsComponent *physicsComponent_;
    };
}

#endif
