#include "monster_collapse_state.hpp"

#include "actor.hpp"
#include "block_physics_component.hpp"
#include "control_component.hpp"
#include "game.hpp"
#include "monster_idle_state.hpp"
#include "convert.hpp"

namespace crust {
    namespace {
        bool isBlock(Actor const *actor)
        {
            return dynamic_cast<BlockPhysicsComponent const *>(actor->getPhysicsComponent()) != 0;
        }
    }

    MonsterCollapseState::MonsterCollapseState(Actor *actor) :
        actor_(actor),
        controlComponent_(convert(actor->getControlComponent())),
        distance_(2.0f)
    { }
    
    std::auto_ptr<State> MonsterCollapseState::transition()
    {
        if (!controlComponent_->getActionControl()) {
            return std::auto_ptr<State>(new MonsterIdleState(actor_));
        }
        return std::auto_ptr<State>();
    }

    void MonsterCollapseState::step(float dt)
    {
        Vector2 targetPosition = controlComponent_->getTargetPosition();
        for (int i = 0; i < actor_->getGame()->getActorCount(); ++i) {
            Actor *actor = actor_->getGame()->getActor(i);
            if (isBlock(actor) && getSquaredDistance(actor->getPhysicsComponent()->getPosition(), targetPosition) < square(distance_)) {
                static_cast<BlockPhysicsComponent *>(actor->getPhysicsComponent())->getBody()->SetType(b2_dynamicBody);
            }
        }
    }
}
