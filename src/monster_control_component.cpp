#include "monster_control_component.hpp"

#include "actor.hpp"
#include "game.hpp"
#include "monster_idle_state.hpp"
#include "monster_physics_component.hpp"
#include "state.hpp"
#include "task.hpp"
#include "wire.hpp"

namespace crust {
    MonsterControlComponent::MonsterControlComponent(Actor *actor) :
        actor_(actor),
        physicsComponent_(wire(actor->getPhysicsComponent())),

        maxVelocity_(5.0f),
        jumpDuration_(0.2f),
        jumpVelocity_(8.0f),
        maxDriftVelocity_(3.0f),
        driftForce_(10.0f),
        maxBoostVelocity_(7.0f),
        boostDuration_(0.3f),
        boostAcceleration_(20.0f),
        jumpTime_(0.0f),

        leftControl_(false),
        rightControl_(false),
        jumpControl_(false),
        actionControl_(false)
    { }

    MonsterControlComponent::~MonsterControlComponent()
    { }

    void MonsterControlComponent::create()
    {
        actionState_.reset(new MonsterIdleState(actor_));
        actionState_->create();
    }
    
    void MonsterControlComponent::destroy()
    {
        actionState_->destroy();
        actionState_.reset();
    }

    void MonsterControlComponent::step(float dt)
    {
        bool standing = physicsComponent_->isStanding();
        int xControl = int(rightControl_) - int(leftControl_);
        b2Vec2 velocity = physicsComponent_->getMainBody()->GetLinearVelocity();
        
        // Run.
        float motorSpeed = maxVelocity_ * float(xControl) / physicsComponent_->getWheelRadius();
        physicsComponent_->getWheelJoint()->SetMotorSpeed(motorSpeed);
        
        // Jump.
        if (standing && jumpControl_ &&
            jumpTime_ + jumpDuration_ < actor_->getGame()->getTime())
        {
            b2Vec2 velocity = physicsComponent_->getMainBody()->GetLinearVelocity();
            velocity.y = jumpVelocity_;
            physicsComponent_->getMainBody()->SetLinearVelocity(velocity);
            jumpTime_ = actor_->getGame()->getTime();
        }
        
        // Boost.
        if (!standing && jumpControl_ && 0.0f < velocity.y &&
            velocity.y < maxBoostVelocity_ &&
            actor_->getGame()->getTime() < jumpTime_ + boostDuration_)
        {
            velocity.y += boostAcceleration_ * dt;
            physicsComponent_->getMainBody()->SetLinearVelocity(velocity);
        }
        
        // Drift.
        if (xControl == 1 && physicsComponent_->getMainBody()->GetLinearVelocity().x < maxDriftVelocity_)
        {
            physicsComponent_->getMainBody()->ApplyForce(b2Vec2(driftForce_, 0.0f), physicsComponent_->getMainBody()->GetPosition());
        }
        if (xControl == -1 &&
            physicsComponent_->getMainBody()->GetLinearVelocity().x > -maxDriftVelocity_)
        {
            physicsComponent_->getMainBody()->ApplyForce(b2Vec2(-driftForce_, 0.0f),
                                                         physicsComponent_->getMainBody()->GetPosition());
        }

        if (actionState_->getTask()) {
            actionState_->getTask()->step(dt);
        }
        std::auto_ptr<State> newActionState = actionState_->transition();
        if (newActionState.get()) {
            actionState_->destroy();
            actionState_ = newActionState;
            actionState_->create();
        }
    }
}
