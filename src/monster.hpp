#ifndef CRUST_MONSTER_HPP
#define CRUST_MONSTER_HPP

#include "actor.hpp"
#include "sprite.hpp"

#include <boost/ptr_container/ptr_array.hpp>
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

        void draw() const;

    private:
        Game *game_;
        b2Body *body_;
        b2Body *wheelBody_;
        b2RevoluteJoint *wheelJoint_;
        b2Fixture *floorSensorFixture_;
        Sprite sprite_;

        float wheelRadius_;
        float maxVelocity_;
        float jumpImpulse_;
        float jumpDuration_;
        float maxDriftVelocity_;
        float driftForce_;
        float maxBoostVelocity_;
        float boostDuration_;
        float boostForce_;
        float jumpTime_;
        int faceDirection_;
        
        bool leftControl_;
        bool rightControl_;
        bool jumpControl_;

        bool isStanding();
    };
}

#endif
