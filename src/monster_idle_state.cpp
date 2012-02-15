#include "monster_idle_state.hpp"

#include "actor.hpp"
#include "control_component.hpp"
#include "monster_dig_state.hpp"
#include "wire.hpp"

namespace crust {
    MonsterIdleState::MonsterIdleState(Actor *actor) :
        actor_(actor),
        controlComponent_(wire(actor->getControlComponent()))
    { }

    std::auto_ptr<State> MonsterIdleState::transition()
    {
        if (controlComponent_->getActionControl()) {
            return std::auto_ptr<State>(new MonsterDigState(actor_));
        } else {
            return std::auto_ptr<State>();
        }
    }
}
