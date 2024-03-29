#include "monster_mine_state.hpp"

#include "actor.hpp"
#include "block_physics_component.hpp"
#include "convert.hpp"
#include "game.hpp"
#include "monster_control_component.hpp"
#include "monster_physics_component.hpp"
#include "monster_idle_state.hpp"
#include "physics_manager.hpp"

#include <Box2D/Box2D.h>

namespace crust {
    namespace {        
        bool isBlock(Actor const *actor)
        {
            return dynamic_cast<BlockPhysicsComponent const *>(actor->getPhysicsComponent());
        }
        
        class MineCallback : public b2RayCastCallback {
        public:
            Actor *actor;
            
            MineCallback() :
                actor(0)
            { }
            
            float32 ReportFixture(b2Fixture *fixture, b2Vec2 const &point,
                                  b2Vec2 const &normal, float32 fraction)
            {
                b2Body *body = fixture->GetBody();
                Actor *tempActor = static_cast<Actor *>(body->GetUserData());
                if (tempActor && isBlock(tempActor)) {
                    actor = tempActor;
                    return fraction;
                } else {
                    return 1.0f;
                }
            }
        };
    }

    MonsterMineState::MonsterMineState(Actor *actor) :
        actor_(actor),
        controlComponent_(convert(actor->getControlComponent())),
        physicsComponent_(convert(actor->getPhysicsComponent())),
        physicsManager_(actor->getGame()->getPhysicsManager()),

        targetActor_(0),
        targetPhysicsComponent_(0)
    { }

    std::auto_ptr<State> MonsterMineState::transition()
    {
        if (!controlComponent_->getActionControl()) {
            return std::auto_ptr<State>(new MonsterIdleState(actor_));
        }
        return std::auto_ptr<State>();
    }

    void MonsterMineState::step(float dt)
    {
        b2Vec2 p1Vec2 = physicsComponent_->getMainBody()->GetPosition();
        Vector2 p1(p1Vec2.x, p1Vec2.y);
        Vector2 p2 = p1 + clampLength(controlComponent_->getTargetPosition() - p1, 1.5f);
        MineCallback callback;
        physicsManager_->getWorld()->RayCast(&callback, b2Vec2(p1.x, p1.y),
                                             b2Vec2(p2.x, p2.y));
        if (callback.actor && callback.actor == targetActor_) {
            float duration = targetPhysicsComponent_->getMineDuration();
            duration += dt;
            targetPhysicsComponent_->setMineDuration(duration);
            if (0.5f < duration) {
                actor_->getGame()->removeActor(callback.actor);
            }
        } else {
            targetActor_ = callback.actor;
            if (targetActor_) {
                targetPhysicsComponent_ = convert(targetActor_->getPhysicsComponent());
            } else {
                targetPhysicsComponent_ = 0;
            }
        }
    }
}
