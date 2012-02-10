#ifndef CRUST_CHAIN_RENDER_COMPONENT_HPP
#define CRUST_CHAIN_RENDER_COMPONENT_HPP

#include "render_component.hpp"

namespace crust {
    class Actor;
    class ChainPhysicsComponent;

    class ChainRenderComponent : public RenderComponent {
    public:
        explicit ChainRenderComponent(Actor *actor);
        void draw() const;

    private:
        Actor *actor_;
        ChainPhysicsComponent *physicsComponent_;
    };
}

#endif
