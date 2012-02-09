#ifndef CRUST_MONSTER_HPP
#define CRUST_MONSTER_HPP

#include "actor.hpp"
#include "geometry.hpp"

#include <memory>
#include <boost/ptr_container/ptr_array.hpp>
#include <Box2D/Box2D.h>

namespace crust {
    class Game;
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

        Vector2 const &getTargetPosition() const
        {
            return targetPosition_;
        }

        void setTargetPosition(Vector2 const &position)
        {
            targetPosition_ = position;
        }
        
        void stepPhysics(float dt);
        void stepAnimation(float dt);

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
        b2Fixture *topSensorFixture_;
        b2Fixture *leftSensorFixture_;
        b2Fixture *bottomSensorFixture_;
        b2Fixture *rightSensorFixture_;

        Vector2 targetPosition_;
        
        float wheelRadius_;
        float maxVelocity_;
        float jumpDuration_;
        float jumpVelocity_;
        float maxDriftVelocity_;
        float driftForce_;
        float maxBoostVelocity_;
        float boostDuration_;
        float boostAcceleration_;
        float jumpTime_;

        int headDirection_;
        int bodyDirection_;
        
        bool leftControl_;
        bool rightControl_;
        bool jumpControl_;

        std::auto_ptr<MonsterRenderComponent> renderComponent_;
        
        void initPhysics(Vector2 const &position);
        
        bool isStanding();
    };
}

#endif
