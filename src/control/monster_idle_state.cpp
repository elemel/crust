#include "monster_idle_state.hpp"

#include "actor.hpp"
#include "convert.hpp"
#include "monster_control_component.hpp"
#include "monster_drag_state.hpp"
#include "monster_drop_state.hpp"
#include "monster_mine_state.hpp"

namespace crust {
    MonsterIdleState::MonsterIdleState(Actor *actor) :
        actor_(actor),
        controlComponent_(convert(actor->getControlComponent()))
    { }

    std::auto_ptr<State> MonsterIdleState::transition()
    {
        if (controlComponent_->getActionControl()) {
            switch (controlComponent_->getActionMode()) {
                case MonsterControlComponent::MINE_MODE:
                    return std::auto_ptr<State>(new MonsterMineState(actor_));

                case MonsterControlComponent::DRAG_MODE:
                    return std::auto_ptr<State>(new MonsterDragState(actor_));

                case MonsterControlComponent::DROP_MODE:
                    return std::auto_ptr<State>(new MonsterDropState(actor_));

                default:
                    break;
            }
        }
        return std::auto_ptr<State>();
    }
}
