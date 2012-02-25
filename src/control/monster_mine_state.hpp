#ifndef CRUST_MONSTER_MINE_STATE_HPP
#define CRUST_MONSTER_MINE_STATE_HPP

#include "state.hpp"
#include "task.hpp"

namespace crust {
    class Actor;
    class BlockPhysicsComponent;
    class MonsterControlComponent;
    class MonsterPhysicsComponent;
    class PhysicsManager;

    class MonsterMineState : public State, public Task {
    public:
        explicit MonsterMineState(Actor *actor);

        void create()
        { }

        void destroy()
        { }
        
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
        MonsterPhysicsComponent *physicsComponent_;
        PhysicsManager *physicsManager_;

        Actor *targetActor_;
        BlockPhysicsComponent *targetPhysicsComponent_;
    };
}

#endif
