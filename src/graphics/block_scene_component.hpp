#ifndef CRUST_BLOCK_SCENE_COMPONENT_HPP
#define CRUST_BLOCK_SCENE_COMPONENT_HPP

#include "component.hpp"
#include "task.hpp"

#include <memory>

namespace crust {
    class Actor;
    class BlockPhysicsComponent;
    class GraphicsManager;
    class Sprite;
    
    class BlockSceneComponent : public Component, public Task {
    public:
        explicit BlockSceneComponent(Actor *actor);
        ~BlockSceneComponent();

        void create();
        void destroy();

        void step(float dt);
        
    private:
        Actor *actor_;
        BlockPhysicsComponent *physicsComponent_;
        GraphicsManager *graphicsManager_;

        std::auto_ptr<Sprite> sprite_;
        
        void initSprite();
    };
}

#endif
