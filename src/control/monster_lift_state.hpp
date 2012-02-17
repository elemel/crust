#ifndef MONSTER_LIFT_STATE_HPP
#define MONSTER_LIFT_STATE_HPP

#include "state.hpp"
#include "task.hpp"

#include <Box2D/Box2D.h>

namespace crust {
    class Actor;
    class Game;
    class MonsterControlComponent;
    class PhysicsService;

    class MonsterLiftState : public State, public Task {
    public:
        explicit MonsterLiftState(Actor *actor);

        void create();
        void destroy();
        
        std::auto_ptr<State> transition();
        
        Task *getTask()
        {
            return this;
        }
        
        Task const *getTask() const
        {
            return this;
        }
        
        void step(float dt);

    private:
        Actor *actor_;
        MonsterControlComponent *controlComponent_;
        Game *game_;
        PhysicsService *physicsService_;

        Actor *targetActor_;
        b2MouseJoint *joint_;

        void liftBlock(bool fixedRotation);
        void releaseBlock();
    };
}

#endif
