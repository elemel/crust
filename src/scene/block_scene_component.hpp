#ifndef CRUST_BLOCK_SCENE_COMPONENT_HPP
#define CRUST_BLOCK_SCENE_COMPONENT_HPP

#include "scene_component.hpp"

#include <memory>

namespace crust {
    class Actor;
    class BlockPhysicsComponent;
    class Sprite;
    
    class BlockSceneComponent : public SceneComponent {
    public:
        explicit BlockSceneComponent(Actor *actor);
        ~BlockSceneComponent();

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
