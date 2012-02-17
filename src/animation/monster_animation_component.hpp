#ifndef CRUST_MONSTER_ANIMATION_COMPONENT_HPP
#define CRUST_MONSTER_ANIMATION_COMPONENT_HPP

#include "animation_component.hpp"

namespace crust {
    class Actor;
    class MonsterControlComponent;
    class MonsterPhysicsComponent;
    class MonsterSceneComponent;

    class MonsterAnimationComponent : public AnimationComponent {
    public:
        MonsterAnimationComponent(Actor *actor);

        void create()
        { }

        void destroy()
        { }
        
        void step(float dt);
        
    private:
        Actor *actor_;
        MonsterPhysicsComponent *physicsComponent_;
        MonsterControlComponent *controlComponent_;
        MonsterSceneComponent *sceneComponent_;

        int headDirection_;
        int trunkDirection_;
    };
}

#endif
