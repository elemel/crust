#include "monster_dig_state.hpp"

#include "actor.hpp"
#include "block_physics_component.hpp"
#include "control_component.hpp"
#include "game.hpp"
#include "monster_idle_state.hpp"
#include "physics_component.hpp"

#include <Box2D/Box2D.h>

namespace crust {
    namespace {        
        bool isBlock(Actor const *actor)
        {
            return dynamic_cast<BlockPhysicsComponent const *>(actor->getPhysicsComponent());
        }
        
        class DigCallback : public b2RayCastCallback {
        public:
            Actor *actor;
            
            DigCallback() :
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

    std::auto_ptr<State> MonsterDigState::transition()
    {
        if (!actor_->getControlComponent()->getActionControl()) {
            return std::auto_ptr<State>(new MonsterIdleState(actor_));
        } else {
            return std::auto_ptr<State>();
        }
    }

    void MonsterDigState::step(float dt)
    {
        Vector2 p1 = actor_->getPhysicsComponent()->getPosition();
        Vector2 p2 = p1 + clampLength(actor_->getControlComponent()->getTargetPosition() - p1, 1.5f);
        DigCallback callback;
        actor_->getGame()->getPhysicsWorld()->RayCast(&callback, b2Vec2(p1.x, p1.y),
                                                      b2Vec2(p2.x, p2.y));
        if (callback.actor) {
            actor_->getGame()->removeActor(callback.actor);
        }
    }
}