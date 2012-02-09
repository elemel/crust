#ifndef CRUST_MONSTER_HPP
#define CRUST_MONSTER_HPP

#include "actor.hpp"

#include <memory>

namespace crust {
    class AnimationComponent;
    class ControlComponent;
    class Game;
    class PhysicsComponent;
    class RenderComponent;
    class Vector2;

    class Monster : public Actor {
    public:
        Monster(Game *game, Vector2 const &position);
        ~Monster();

        Game *getGame()
        {
            return game_;
        }

        Game const *getGame() const
        {
            return game_;
        }

        PhysicsComponent *getPhysicsComponent()
        {
            return physicsComponent_.get();
        }
        
        PhysicsComponent const *getPhysicsComponent() const
        {
            return physicsComponent_.get();
        }

        ControlComponent *getControlComponent()
        {
            return controlComponent_.get();
        }

        ControlComponent const *getControlComponent() const
        {
            return controlComponent_.get();
        }

        RenderComponent *getRenderComponent()
        {
            return renderComponent_.get();
        }
        
        RenderComponent const *getRenderComponent() const
        {
            return renderComponent_.get();
        }

        AnimationComponent *getAnimationComponent()
        {
            return animationComponent_.get();
        }
        
        AnimationComponent const *getAnimationComponent() const
        {
            return animationComponent_.get();
        }

    private:
        Game *game_;
        
        std::auto_ptr<PhysicsComponent> physicsComponent_;
        std::auto_ptr<ControlComponent> controlComponent_;
        std::auto_ptr<RenderComponent> renderComponent_;
        std::auto_ptr<AnimationComponent> animationComponent_;
    };
}

#endif
