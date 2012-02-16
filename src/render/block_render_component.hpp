#ifndef CRUST_BLOCK_RENDER_COMPONENT_HPP
#define CRUST_BLOCK_RENDER_COMPONENT_HPP

#include "render_component.hpp"

#include <memory>

namespace crust {
    class Actor;
    class BlockPhysicsComponent;
    class Sprite;
    
    class BlockRenderComponent : public RenderComponent {
    public:
        explicit BlockRenderComponent(Actor *actor);
        ~BlockRenderComponent();

        void create()
        { }

        void destroy()
        { }

        void draw() const;
        
    private:
        Actor *actor_;
        BlockPhysicsComponent *physicsComponent_;

        mutable bool spriteDirty_;
        mutable std::auto_ptr<Sprite> sprite_;
        
        void updateSprite() const;
    };
}

#endif
