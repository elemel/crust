#include "monster_control_component.hpp"

#include "game.hpp"
#include "monster.hpp"
#include "monster_physics_component.hpp"

namespace crust {
    MonsterControlComponent::MonsterControlComponent(Monster *monster,
                                                     MonsterPhysicsComponent *physicsComponent) :
        monster_(monster),
        physicsComponent_(physicsComponent),

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
        jumpControl_(false)
    { }

    void MonsterControlComponent::step(float dt)
    {
        Game *game = monster_->getGame();

        bool standing = physicsComponent_->isStanding();
        int xControl = int(rightControl_) - int(leftControl_);
        b2Vec2 velocity = physicsComponent_->getMainBody()->GetLinearVelocity();
        
        // Run.
        float motorSpeed = maxVelocity_ * float(xControl) / physicsComponent_->getWheelRadius();
        physicsComponent_->getWheelJoint()->SetMotorSpeed(motorSpeed);
        
        // Jump.
        if (standing && jumpControl_ &&
            jumpTime_ + jumpDuration_ < game->getTime())
        {
            b2Vec2 velocity = physicsComponent_->getMainBody()->GetLinearVelocity();
            velocity.y = jumpVelocity_;
            physicsComponent_->getMainBody()->SetLinearVelocity(velocity);
            jumpTime_ = game->getTime();
        }
        
        // Boost.
        if (!standing && jumpControl_ && 0.0f < velocity.y &&
            velocity.y < maxBoostVelocity_ &&
            game->getTime() < jumpTime_ + boostDuration_)
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
    }
}
