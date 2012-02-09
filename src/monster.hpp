#ifndef CRUST_MONSTER_HPP
#define CRUST_MONSTER_HPP

#include "actor.hpp"
#include "geometry.hpp"

#include <memory>
#include <boost/ptr_container/ptr_array.hpp>

namespace crust {
    class Game;
    class MonsterControlComponent;
    class MonsterPhysicsComponent;
    class MonsterRenderComponent;
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

        Vector2 getPosition() const;

        void stepAnimation(float dt);

        void draw() const;

        MonsterControlComponent *getControlComponent()
        {
            return controlComponent_.get();
        }

        MonsterControlComponent const *getControlComponent() const
        {
            return controlComponent_.get();
        }

    private:
        Game *game_;
        
        int headDirection_;
        int bodyDirection_;
        
        std::auto_ptr<MonsterPhysicsComponent> physicsComponent_;
        std::auto_ptr<MonsterControlComponent> controlComponent_;
        std::auto_ptr<MonsterRenderComponent> renderComponent_;
    };
}

#endif
