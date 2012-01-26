#include "monster.hpp"

#include "debug_draw.hpp"
#include "game.hpp"
#include "geometry.hpp"
#include "ray_spring.hpp"

namespace crust {
    Monster::Monster(Game *game, Vector2 const &position) :
        game_(game),
        body_(0),

        wheelRadius_(0.3f),
        maxVelocity_(5.0f),
        jumpImpulse_(6.0f),
        jumpDuration_(0.2f),
        maxDriftVelocity_(3.0f),
        driftForce_(10.0f),
        maxBoostVelocity_(7.0f),
        boostDuration_(0.3f),
        boostForce_(15.0f),
        jumpTime_(0.0f),

        leftControl_(false),
        rightControl_(false),
        jumpControl_(false)
    {
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.fixedRotation = true;
        bodyDef.position.Set(position.x, position.y);
        bodyDef.userData = static_cast<Actor *>(this);
        body_ = game_->getPhysicsWorld()->CreateBody(&bodyDef);

        b2CircleShape shape;
        shape.m_radius = 0.5f;
        body_->CreateFixture(&shape, 1.0f);

        b2BodyDef wheelBodyDef;
        wheelBodyDef.type = b2_dynamicBody;
        wheelBodyDef.position.Set(position.x, position.y - 0.5f);
        wheelBodyDef.userData = static_cast<Actor *>(this);
        wheelBody_ = game_->getPhysicsWorld()->CreateBody(&wheelBodyDef);
        
        b2CircleShape wheelShape;
        wheelShape.m_radius = wheelRadius_;
        b2Fixture *wheelFixture = wheelBody_->CreateFixture(&wheelShape, 1.0f);
        wheelFixture->SetFriction(10.0f);
        wheelFixture->SetRestitution(0.0f);

        b2RevoluteJointDef wheelJointDef;
        wheelJointDef.Initialize(wheelBody_, body_, wheelBody_->GetPosition());
        wheelJointDef.enableMotor = true;
        wheelJointDef.maxMotorTorque = 10.0f;
        wheelJoint_ = static_cast<b2RevoluteJoint *>(game_->getPhysicsWorld()->CreateJoint(&wheelJointDef));

        b2CircleShape floorSensorShape;
        floorSensorShape.m_radius = wheelRadius_ + 0.1f;
        floorSensorFixture_ = wheelBody_->CreateFixture(&floorSensorShape,
                                                        0.0f);
        floorSensorFixture_->SetSensor(true);
    }

    Monster::~Monster()
    {
        game_->getPhysicsWorld()->DestroyJoint(wheelJoint_);
        game_->getPhysicsWorld()->DestroyBody(wheelBody_);
        game_->getPhysicsWorld()->DestroyBody(body_);
    }

    Vector2 Monster::getPosition() const
    {
        b2Vec2 position = body_->GetPosition();
        return Vector2(position.x, position.y);
    }

    void Monster::step(float dt)
    {
        bool standing = isStanding();
        int xControl = int(rightControl_) - int(leftControl_);
        b2Vec2 velocity = body_->GetLinearVelocity();

        // Run.
        wheelJoint_->SetMotorSpeed(maxVelocity_ * float(xControl)
                                   / wheelRadius_);

        // Jump.
        if (standing && jumpControl_ &&
            jumpTime_ + jumpDuration_ < game_->getTime())
        {
            b2Vec2 impulse(0.0f, jumpImpulse_);
            body_->ApplyLinearImpulse(impulse, body_->GetPosition());
            jumpTime_ = game_->getTime();
        }

        // Boost.
        if (!standing && jumpControl_ && 0.0f < velocity.y &&
            velocity.y < maxBoostVelocity_ &&
            game_->getTime() < jumpTime_ + boostDuration_)
        {
            body_->ApplyForce(b2Vec2(0.0f, boostForce_), body_->GetPosition());
        }

        // Drift.
        if (xControl == 1 && body_->GetLinearVelocity().x < maxDriftVelocity_)
        {
            body_->ApplyForce(b2Vec2(driftForce_, 0.0f), body_->GetPosition());
        }
        if (xControl == -1 &&
            body_->GetLinearVelocity().x > -maxDriftVelocity_)
        {
            body_->ApplyForce(b2Vec2(-driftForce_, 0.0f),
                              body_->GetPosition());
        }
    }

    void Monster::debugDraw() const
    {
        b2Vec2 position = body_->GetPosition();
        debugDrawCircle(position.x, position.y, 0.5f);

        b2Vec2 wheelPosition = wheelBody_->GetPosition();
        debugDrawCircle(wheelPosition.x, wheelPosition.y, wheelRadius_);
    }

    bool Monster::isStanding()
    {
        for (b2ContactEdge *edge = wheelBody_->GetContactList(); edge;
             edge = edge->next)
        {
            if (edge->contact->IsTouching()) {
                return true;
            }
        }
        return false;
    }
}
