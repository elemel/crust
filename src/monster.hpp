#ifndef CRUST_MONSTER_HPP
#define CRUST_MONSTER_HPP

#include "actor.hpp"

#include <Box2D/Box2D.h>

namespace crust {
    class Game;
    class Vector2;

    class Monster : public Actor {
    public:
        Monster(Game *game, Vector2 const &position);
        ~Monster();

        Vector2 getPosition() const;
        
        void step(float dt);

        void setLeftControl(bool control)
        {
            leftControl_ = control;
        }

        void setRightControl(bool control)
        {
            rightControl_ = control;
        }

        void setJumpControl(bool control)
        {
            jumpControl_ = control;
        }

    private:
        Game *game_;
        b2Body *body_;

        bool leftControl_;
        bool rightControl_;
        bool jumpControl_;
    };
}

#endif
