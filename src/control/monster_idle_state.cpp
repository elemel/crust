#include "monster_idle_state.hpp"

#include "actor.hpp"
#include "convert.hpp"
#include "monster_collapse_state.hpp"
#include "monster_control_component.hpp"
#include "monster_dig_state.hpp"
#include "monster_lift_state.hpp"

namespace crust {
    MonsterIdleState::MonsterIdleState(Actor *actor) :
        actor_(actor),
        controlComponent_(convert(actor->getControlComponent()))
    { }

    std::auto_ptr<State> MonsterIdleState::transition()
    {
        if (controlComponent_->getActionControl()) {
            switch (controlComponent_->getActionMode()) {
                case MonsterControlComponent::DIG_MODE:
                    return std::auto_ptr<State>(new MonsterDigState(actor_));

                case MonsterControlComponent::LIFT_MODE:
                    return std::auto_ptr<State>(new MonsterLiftState(actor_));

                case MonsterControlComponent::COLLAPSE_MODE:
                    return std::auto_ptr<State>(new MonsterCollapseState(actor_));

                default:
                    break;
            }
        }
        return std::auto_ptr<State>();
    }
}
