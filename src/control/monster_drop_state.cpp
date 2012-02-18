#include "monster_drop_state.hpp"

#include "actor.hpp"
#include "block_physics_component.hpp"
#include "game.hpp"
#include "monster_control_component.hpp"
#include "monster_idle_state.hpp"
#include "convert.hpp"

namespace crust {
    namespace {
        bool isBlock(Actor const *actor)
        {
            return dynamic_cast<BlockPhysicsComponent const *>(actor->getPhysicsComponent()) != 0;
        }
    }

    MonsterDropState::MonsterDropState(Actor *actor) :
        actor_(actor),
        controlComponent_(convert(actor->getControlComponent())),
        distance_(2.0f)
    { }
    
    std::auto_ptr<State> MonsterDropState::transition()
    {
        if (!controlComponent_->getActionControl()) {
            return std::auto_ptr<State>(new MonsterIdleState(actor_));
        }
        return std::auto_ptr<State>();
    }

    void MonsterDropState::step(float dt)
    {
        Vector2 targetPosition = controlComponent_->getTargetPosition();
        for (int i = 0; i < actor_->getGame()->getActorCount(); ++i) {
            Actor *tempActor = actor_->getGame()->getActor(i);
            if (isBlock(tempActor)) {
                BlockPhysicsComponent *tempPhysicsComponent = convert(tempActor->getPhysicsComponent());
                b2Body *tempBody = tempPhysicsComponent->getBody();
                b2Vec2 tempPositionVec2 = tempBody->GetPosition();
                Vector2 tempPosition(tempPositionVec2.x, tempPositionVec2.y);
                if (getSquaredDistance(tempPosition, targetPosition) < square(distance_)) {
                    tempBody->SetType(b2_dynamicBody);
                }
            }
        }
    }
}
