#ifndef CRUST_MONSTER_COLLAPSE_STATE_HPP
#define CRUST_MONSTER_COLLAPSE_STATE_HPP

#include "state.hpp"
#include "task.hpp"

namespace crust {
    class Actor;
    class ControlComponent;
    
    class MonsterCollapseState : public State, public Task {
    public:
        explicit MonsterCollapseState(Actor *actor);
        
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
        ControlComponent *controlComponent_;
        float distance_;
    };
}

#endif
