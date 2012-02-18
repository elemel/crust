#include "monster_drag_state.hpp"

#include "actor.hpp"
#include "block_physics_component.hpp"
#include "convert.hpp"
#include "game.hpp"
#include "monster_control_component.hpp"
#include "monster_idle_state.hpp"
#include "physics_service.hpp"

namespace crust {
    namespace {
        bool isBlock(Actor const *actor)
        {
            return dynamic_cast<BlockPhysicsComponent const *>(actor->getPhysicsComponent());
        }
    }

    MonsterDragState::MonsterDragState(Actor *actor) :
        actor_(actor),
        controlComponent_(convert(actor->getControlComponent())),
        game_(actor->getGame()),
        physicsService_(actor->getGame()->getPhysicsService()),
    
        targetActor_(0),
        joint_(0)
    { }

    void MonsterDragState::create()
    {
        liftBlock(true);
    }

    void MonsterDragState::destroy()
    {
        releaseBlock();
    }

    std::auto_ptr<State> MonsterDragState::transition()
    {
        if (!controlComponent_->getActionControl()) {
            return std::auto_ptr<State>(new MonsterIdleState(actor_));
        }
        return std::auto_ptr<State>();
    }

    void MonsterDragState::step(float dt)
    {
        liftBlock(false);
        if (targetActor_) {
            BlockPhysicsComponent *physicsComponent = convert(targetActor_->getPhysicsComponent());
            b2Body *body = physicsComponent->getBody();
            b2Vec2 velocity = body->GetLinearVelocity();
            Vector2 targetPosition = controlComponent_->getTargetPosition();
            float maxVelocity = 5.0f * b2Distance(joint_->GetAnchorB(), b2Vec2(targetPosition.x, targetPosition.y));
            maxVelocity = std::max(maxVelocity, 5.0f);
            if (square(maxVelocity) < velocity.LengthSquared()) {
                velocity.Normalize();
                velocity *= maxVelocity;
                body->SetLinearVelocity(velocity);
            }
        }
        if (joint_) {
            Vector2 targetPosition = controlComponent_->getTargetPosition();
            joint_->SetTarget(b2Vec2(targetPosition.x, targetPosition.y));
        }
    }

    void MonsterDragState::liftBlock(bool fixedRotation)
    {
        if (targetActor_) {
            return;
        }

        Vector2 targetPosition = controlComponent_->getTargetPosition();

        for (int i = 0; i < game_->getActorCount(); ++i) {
            Actor *actor = game_->getActor(i);
            if (isBlock(actor)) {
                BlockPhysicsComponent *physicsComponent = convert(actor->getPhysicsComponent());
                if (physicsComponent->containsPoint(targetPosition)) {
                    targetActor_ = actor;
                }
            }
        }
        
        if (targetActor_) {
            BlockPhysicsComponent *physicsComponent = convert(targetActor_->getPhysicsComponent());
            b2Body *body = physicsComponent->getBody();
            
            b2Vec2 localPointVec2 = body->GetLocalPoint(b2Vec2(targetPosition.x, targetPosition.y));
            Vector2 localPoint(localPointVec2.x, localPointVec2.y);
            Polygon2 const &localPolygon = physicsComponent->getLocalPolygon();
            Vector2 localCentroid = localPolygon.getCentroid();
            
            if (fixedRotation) {
                body->SetAngularVelocity(0.0f);
                body->SetFixedRotation(true);
            }
            body->SetType(b2_dynamicBody);
            body->SetSleepingAllowed(false);
            
            b2MouseJointDef jointDef;
            jointDef.target.Set(targetPosition.x, targetPosition.y);
            jointDef.bodyA = body;
            jointDef.bodyB = body;
            jointDef.maxForce = 5.0f * body->GetMass() * 10.0f;
            joint_ = static_cast<b2MouseJoint *>(physicsService_->getWorld()->CreateJoint(&jointDef));
        }
    }
    
    void MonsterDragState::releaseBlock()
    {
        if (targetActor_ == 0) {
            return;
        }

        BlockPhysicsComponent *physicsComponent = convert(targetActor_->getPhysicsComponent());
        b2Body *body = physicsComponent->getBody();
        bool makeStatic = false;
        b2Vec2 linearVelocity = body->GetLinearVelocityFromWorldPoint(joint_->GetTarget());
        float angularVelocity = body->GetAngularVelocity();
        if (linearVelocity.LengthSquared() < square(0.1f) &&
            std::abs(angularVelocity) < 0.1f)
        {
            for (b2ContactEdge *i = body->GetContactList(); i; i = i->next) {
                if (i->contact->IsTouching() && i->other->GetType() == b2_staticBody) {
                    makeStatic = true;
                }
            }
        }
        if (makeStatic) {
            body->SetType(b2_staticBody);
        }
        body->SetFixedRotation(false);
        body->SetSleepingAllowed(true);
        
        physicsService_->getWorld()->DestroyJoint(joint_);
        joint_ = 0;
        targetActor_ = 0;
    }
}
