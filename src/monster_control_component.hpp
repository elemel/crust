#ifndef CRUST_MONSTER_CONTROL_COMPONENT_HPP
#define CRUST_MONSTER_CONTROL_COMPONENT_HPP

#include "control_component.hpp"
#include "geometry.hpp"

namespace crust {
    class Actor;
    class MonsterPhysicsComponent;
    
    class MonsterControlComponent : public ControlComponent {
    public:
        explicit MonsterControlComponent(Actor *actor);

        bool getLeftControl() const
        {
            return leftControl_;
        }
        
        void setLeftControl(bool control)
        {
            leftControl_ = control;
        }

        bool getRightControl() const
        {
            return rightControl_;
        }

        void setRightControl(bool control)
        {
            rightControl_ = control;
        }

        bool getJumpControl() const
        {
            return jumpControl_;
        }
        
        void setJumpControl(bool control)
        {
            jumpControl_ = control;
        }

        Vector2 const &getTargetPosition() const
        {
            return targetPosition_;
        }
        
        void setTargetPosition(Vector2 const &position)
        {
            targetPosition_ = position;
        }

        void step(float dt);

    private:
        Actor *actor_;
        MonsterPhysicsComponent *physicsComponent_;

        float maxVelocity_;
        float jumpDuration_;
        float jumpVelocity_;
        float maxDriftVelocity_;
        float driftForce_;
        float maxBoostVelocity_;
        float boostDuration_;
        float boostAcceleration_;
        float jumpTime_;

        bool leftControl_;
        bool rightControl_;
        bool jumpControl_;
        
        Vector2 targetPosition_;
    };
}

#endif