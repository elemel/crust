#ifndef CRUST_MONSTER_GRAPHICS_COMPONENT_HPP
#define CRUST_MONSTER_GRAPHICS_COMPONENT_HPP

#include "component.hpp"
#include "task.hpp"

#include <memory>

namespace crust {
    class Actor;
    class GraphicsManager;
    class MonsterControlComponent;
    class MonsterPhysicsComponent;
    class Sprite;

    class MonsterGraphicsComponent : public Component, public Task {
    public:
        explicit MonsterGraphicsComponent(Actor *actor);
        ~MonsterGraphicsComponent();

        void create();
        void destroy();

        void step(float dt);

        Sprite *getHeadSprite()
        {
            return headSprite_.get();
        }

        Sprite *getTrunkSprite()
        {
            return trunkSprite_.get();
        }

    private:
        Actor *actor_;
        MonsterControlComponent *controlComponent_;
        MonsterPhysicsComponent *physicsComponent_;
        GraphicsManager *graphicsManager_;

        int headDirection_;
        int trunkDirection_;

        std::auto_ptr<Sprite> headSprite_;
        std::auto_ptr<Sprite> trunkSprite_;
        std::auto_ptr<Sprite> leftArmSprite_;
        std::auto_ptr<Sprite> rightArmSprite_;
        std::auto_ptr<Sprite> leftLegSprite_;
        std::auto_ptr<Sprite> rightLegSprite_;

        void initSprites();
    };
}

#endif
