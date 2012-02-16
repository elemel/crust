#ifndef CRUST_MONSTER_DIG_STATE_HPP
#define CRUST_MONSTER_DIG_STATE_HPP

#include "state.hpp"
#include "task.hpp"

namespace crust {
    class Actor;
    class MonsterControlComponent;
    class MonsterPhysicsComponent;

    class MonsterDigState : public State, public Task {
    public:
        explicit MonsterDigState(Actor *actor);

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
    };
}

#endif
