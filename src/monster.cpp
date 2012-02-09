#include "monster.hpp"

#include "debug_draw.hpp"
#include "game.hpp"
#include "geometry.hpp"
#include "monster_physics_component.hpp"
#include "monster_render_component.hpp"
#include "sprite.hpp"

namespace crust {
    Monster::Monster(Game *game, Vector2 const &position) :
        game_(game),

        maxVelocity_(5.0f),
        jumpDuration_(0.2f),
        jumpVelocity_(8.0f),
        maxDriftVelocity_(3.0f),
        driftForce_(10.0f),
        maxBoostVelocity_(7.0f),
        boostDuration_(0.3f),
        boostAcceleration_(20.0f),
        jumpTime_(0.0f),

        headDirection_(1),
        bodyDirection_(1),

        leftControl_(false),
        rightControl_(false),
        jumpControl_(false),
    
        physicsComponent_(new MonsterPhysicsComponent(this, position)),
        renderComponent_(new MonsterRenderComponent(this))
    { }

    Monster::~Monster()
    { }

    Vector2 Monster::getPosition() const
    {
        b2Vec2 position = physicsComponent_->getMainBody()->GetPosition();
        return Vector2(position.x, position.y);
    }

    void Monster::stepPhysics(float dt)
    {
        bool standing = physicsComponent_->isStanding();
        int xControl = int(rightControl_) - int(leftControl_);
        b2Vec2 velocity = physicsComponent_->getMainBody()->GetLinearVelocity();

        // Run.
        float motorSpeed = maxVelocity_ * float(xControl) / physicsComponent_->getWheelRadius();
        physicsComponent_->getWheelJoint()->SetMotorSpeed(motorSpeed);

        // Jump.
        if (standing && jumpControl_ &&
            jumpTime_ + jumpDuration_ < game_->getTime())
        {
            b2Vec2 velocity = physicsComponent_->getMainBody()->GetLinearVelocity();
            velocity.y = jumpVelocity_;
            physicsComponent_->getMainBody()->SetLinearVelocity(velocity);
            jumpTime_ = game_->getTime();
        }

        // Boost.
        if (!standing && jumpControl_ && 0.0f < velocity.y &&
            velocity.y < maxBoostVelocity_ &&
            game_->getTime() < jumpTime_ + boostDuration_)
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

    void Monster::stepAnimation(float dt)
    {
        int xControl = int(rightControl_) - int(leftControl_);
        bool bodyTurned = false;
        if (xControl && xControl != bodyDirection_) {
            bodyDirection_ = xControl;
            renderComponent_->getBodySprite()->setScale(Vector2(0.1f * float(bodyDirection_), 0.1f));
            bodyTurned = true;
        }
        b2Vec2 localEyePosition = b2Vec2(0.0f, 0.35f);
        b2Vec2 localTargetPosition = physicsComponent_->getMainBody()->GetLocalPoint(b2Vec2(targetPosition_.x, targetPosition_.y));
        if (bodyTurned || 0.05f < std::abs(localTargetPosition.x)) {
            headDirection_ = (localTargetPosition.x < 0) ? -1 : 1;
        }
        if (bodyDirection_ == -1) {
            localTargetPosition.x = -localTargetPosition.x;
        }
        if (headDirection_ != bodyDirection_) {
            localTargetPosition.x = -localTargetPosition.x;
        }
        b2Vec2 eyeToTargetOffset = localTargetPosition - localEyePosition;
        float targetAngle = std::atan2(eyeToTargetOffset.y, eyeToTargetOffset.x);
        float headAngle = 0.5f * float(headDirection_) * targetAngle;
        renderComponent_->getHeadSprite()->setAngle(headAngle);
        renderComponent_->getHeadSprite()->setScale(Vector2(0.1f * float(headDirection_), 0.1f));
    }

    void Monster::draw() const
    {
        glPushMatrix();
        b2Vec2 const &position = physicsComponent_->getMainBody()->GetPosition();
        float angle = physicsComponent_->getMainBody()->GetAngle();
        glTranslatef(position.x, position.y, 0.0f);
        glRotatef((180.0f / M_PI) * angle, 0.0f, 0.0f, 1.0f);
        renderComponent_->draw();
        glPopMatrix();
    }
}
