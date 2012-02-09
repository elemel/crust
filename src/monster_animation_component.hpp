#ifndef CRUST_MONSTER_ANIMATION_COMPONENT_HPP
#define CRUST_MONSTER_ANIMATION_COMPONENT_HPP

#include "animation_component.hpp"

namespace crust {
    class Monster;
    class MonsterControlComponent;
    class MonsterPhysicsComponent;
    class MonsterRenderComponent;

    class MonsterAnimationComponent : public AnimationComponent {
    public:
        MonsterAnimationComponent(Monster *monster,
                                  MonsterPhysicsComponent *physicsComponent,
                                  MonsterControlComponent *controlComponent,
                                  MonsterRenderComponent *renderComponent);

        void step(float dt);
        
    private:
        Monster *monster_;
        MonsterPhysicsComponent *physicsComponent_;
        MonsterControlComponent *controlComponent_;
        MonsterRenderComponent *renderComponent_;

        int headDirection_;
        int trunkDirection_;
    };
}

#endif
