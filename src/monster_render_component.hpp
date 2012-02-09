#ifndef CRUST_MONSTER_RENDER_COMPONENT_HPP
#define CRUST_MONSTER_RENDER_COMPONENT_HPP

#include "render_component.hpp"

#include <memory>

namespace crust {
    class Actor;
    class MonsterPhysicsComponent;
    class Sprite;

    class MonsterRenderComponent : public RenderComponent {
    public:
        explicit MonsterRenderComponent(Actor *actor);

        Sprite *getHeadSprite()
        {
            return headSprite_.get();
        }

        Sprite *getTrunkSprite()
        {
            return trunkSprite_.get();
        }

        void draw() const;

    private:
        Actor *actor_;
        MonsterPhysicsComponent *physicsComponent_;

        std::auto_ptr<Sprite> headSprite_;
        std::auto_ptr<Sprite> trunkSprite_;

        void initSprites();
    };
}

#endif
