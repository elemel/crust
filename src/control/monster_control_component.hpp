#ifndef CRUST_MONSTER_CONTROL_COMPONENT_HPP
#define CRUST_MONSTER_CONTROL_COMPONENT_HPP

#include "component.hpp"
#include "geometry.hpp"
#include "task.hpp"

namespace crust {
    class Actor;
    class ControlService;
    class MonsterPhysicsComponent;
    class State;
    
    class MonsterControlComponent : public Component, public Task {
    public:
        enum ActionMode {
            MINE_MODE,
            DRAG_MODE,
            DROP_MODE,
            
            ACTION_MODE_COUNT
        };
        
        explicit MonsterControlComponent(Actor *actor);
        ~MonsterControlComponent();

        void create();
        void destroy();
        
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

        bool getActionControl() const
        {
            return actionControl_;
        }

        void setActionControl(bool control)
        {
            actionControl_ = control;
        }
        
        Vector2 const &getTargetPosition() const
        {
            return targetPosition_;
        }
        
        void setTargetPosition(Vector2 const &position)
        {
            targetPosition_ = position;
        }

        ActionMode getActionMode() const
        {
            return actionMode_;
        }

        void setActionMode(ActionMode mode)
        {
            actionMode_ = mode;
        }
        
        void step(float dt);

    private:
        Actor *actor_;
        MonsterPhysicsComponent *physicsComponent_;
        ControlService *controlService_;

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
        bool actionControl_;

        Vector2 targetPosition_;
        ActionMode actionMode_;

        std::auto_ptr<State> actionState_;
    };
}

#endif
