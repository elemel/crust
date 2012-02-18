#ifndef CRUST_MONSTER_DROP_STATE_HPP
#define CRUST_MONSTER_DROP_STATE_HPP

#include "state.hpp"
#include "task.hpp"

namespace crust {
    class Actor;
    class MonsterControlComponent;
    
    class MonsterDropState : public State, public Task {
    public:
        explicit MonsterDropState(Actor *actor);
        
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
        float distance_;
    };
}

#endif
