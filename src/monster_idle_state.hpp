#ifndef CRUST_MONSTER_IDLE_STATE_HPP
#define CRUST_MONSTER_IDLE_STATE_HPP

#include "state.hpp"

namespace crust {
    class Actor;
    class ControlComponent;
    
    class MonsterIdleState : public State {
    public:
        explicit MonsterIdleState(Actor *actor);
        
        void create()
        { }
        
        void destroy()
        { }
        
        std::auto_ptr<State> transition();
        
        Task *getTask()
        {
            return 0;
        }
        
        Task const *getTask() const
        {
            return 0;
        }
        
    private:
        Actor *actor_;
        ControlComponent *controlComponent_;
    };
}

#endif
