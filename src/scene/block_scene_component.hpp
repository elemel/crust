#ifndef CRUST_BLOCK_SCENE_COMPONENT_HPP
#define CRUST_BLOCK_SCENE_COMPONENT_HPP

#include "scene_component.hpp"
#include "task.hpp"

#include <memory>

namespace crust {
    class Actor;
    class BlockPhysicsComponent;
    class SceneService;
    class Sprite;
    
    class BlockSceneComponent : public SceneComponent, public Task {
    public:
        explicit BlockSceneComponent(Actor *actor);
        ~BlockSceneComponent();

        void create();
        void destroy();

        void step(float dt);
        
    private:
        Actor *actor_;
        BlockPhysicsComponent *physicsComponent_;
        SceneService *sceneService_;

        std::auto_ptr<Sprite> sprite_;
        
        void initSprite();
    };
}

#endif
