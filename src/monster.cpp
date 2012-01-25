#include "monster.hpp"

#include "debug_draw.hpp"
#include "game.hpp"
#include "geometry.hpp"
#include "ray_spring.hpp"

namespace crust {
    Monster::Monster(Game *game, Vector2 const &position) :
        game_(game),
        body_(0),

        wheelRadius_(0.4f),
        maxVelocity_(5.0f),

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

        b2CircleShape circleShape;
        circleShape.m_radius = 0.5f;
        body_->CreateFixture(&circleShape, 1.0f);

        b2BodyDef wheelBodyDef;
        wheelBodyDef.type = b2_dynamicBody;
        wheelBodyDef.position.Set(position.x, position.y - 0.4f);
        wheelBodyDef.userData = static_cast<Actor *>(this);
        wheelBody_ = game_->getPhysicsWorld()->CreateBody(&wheelBodyDef);
        
        b2CircleShape wheelCircleShape;
        wheelCircleShape.m_radius = wheelRadius_;
        b2Fixture *wheelFixture = wheelBody_->CreateFixture(&wheelCircleShape, 1.0f);
        wheelFixture->SetFriction(10.0f);
        wheelFixture->SetRestitution(0.0f);

        b2RevoluteJointDef jointDef;
        jointDef.Initialize(wheelBody_, body_, wheelBody_->GetPosition());
        jointDef.enableMotor = true;
        jointDef.maxMotorTorque = 10.0f;
        joint_ = static_cast<b2RevoluteJoint *>(game_->getPhysicsWorld()->CreateJoint(&jointDef));
    }

    Monster::~Monster()
    {
        game_->getPhysicsWorld()->DestroyJoint(joint_);
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
        float velocity = maxVelocity_ * (float(rightControl_) - float(leftControl_));
        joint_->SetMotorSpeed(velocity / wheelRadius_);
    }

    void Monster::debugDraw() const
    {
        b2Vec2 position = body_->GetPosition();
        debugDrawCircle(position.x, position.y, 0.5f);

        b2Vec2 wheelPosition = wheelBody_->GetPosition();
        debugDrawCircle(wheelPosition.x, wheelPosition.y, wheelRadius_);
    }
}
