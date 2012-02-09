#include "monster_physics_component.hpp"

#include "actor.hpp"
#include "game.hpp"

namespace crust {
    MonsterPhysicsComponent::MonsterPhysicsComponent(Actor *actor,
                                                     Vector2 const &position) :
        actor_(actor),
    
        wheelRadius_(0.4f),

        mainBody_(0),
        wheelBody_(0),
    
        wheelJoint_(0),

        topSensorFixture_(0),
        leftSensorFixture_(0),
        bottomSensorFixture_(0),
        rightSensorFixture_(0)
    {
        initPhysics(position);
    }

    MonsterPhysicsComponent::~MonsterPhysicsComponent()
    {
        b2World *world = actor_->getGame()->getPhysicsWorld();

        world->DestroyJoint(wheelJoint_);
        world->DestroyBody(wheelBody_);
        world->DestroyBody(mainBody_);
    }

    Vector2 MonsterPhysicsComponent::getPosition() const
    {
        b2Vec2 const &position = mainBody_->GetPosition();
        return Vector2(position.x, position.y);
    }

    float MonsterPhysicsComponent::getAngle() const
    {
        return mainBody_->GetAngle();
    }

    void MonsterPhysicsComponent::initPhysics(Vector2 const &position)
    {
        b2World *world = actor_->getGame()->getPhysicsWorld();

        b2BodyDef mainBodyDef;
        mainBodyDef.type = b2_dynamicBody;
        mainBodyDef.fixedRotation = true;
        mainBodyDef.position.Set(position.x, position.y);
        mainBodyDef.userData = actor_;
        mainBody_ = world->CreateBody(&mainBodyDef);
        
        b2CircleShape shape;
        shape.m_radius = 0.5f;
        mainBody_->CreateFixture(&shape, 1.0f);
        
        b2BodyDef wheelBodyDef;
        wheelBodyDef.type = b2_dynamicBody;
        wheelBodyDef.position.Set(position.x, position.y - 0.4f);
        wheelBodyDef.userData = actor_;
        wheelBody_ = world->CreateBody(&wheelBodyDef);
        
        b2CircleShape wheelShape;
        wheelShape.m_radius = wheelRadius_;
        b2Fixture *wheelFixture = wheelBody_->CreateFixture(&wheelShape, 1.0f);
        wheelFixture->SetFriction(10.0f);
        wheelFixture->SetRestitution(0.0f);
        
        b2RevoluteJointDef wheelJointDef;
        wheelJointDef.Initialize(wheelBody_, mainBody_, wheelBody_->GetPosition());
        wheelJointDef.enableMotor = true;
        wheelJointDef.maxMotorTorque = 10.0f;
        wheelJoint_ = static_cast<b2RevoluteJoint *>(world->CreateJoint(&wheelJointDef));
        
        b2CircleShape topSensorShape;
        topSensorShape.m_p.Set(0.0f, 0.2f);
        topSensorShape.m_radius = wheelRadius_;
        topSensorFixture_ = mainBody_->CreateFixture(&topSensorShape, 0.0f);
        topSensorFixture_->SetSensor(true);
        
        b2CircleShape leftSensorShape;
        leftSensorShape.m_p.Set(-0.2f, 0.0f);
        leftSensorShape.m_radius = wheelRadius_;
        leftSensorFixture_ = mainBody_->CreateFixture(&leftSensorShape, 0.0f);
        leftSensorFixture_->SetSensor(true);
        
        b2CircleShape bottomSensorShape;
        bottomSensorShape.m_p.Set(0.0f, -0.5f);
        bottomSensorShape.m_radius = wheelRadius_;
        bottomSensorFixture_ = mainBody_->CreateFixture(&bottomSensorShape, 0.0f);
        bottomSensorFixture_->SetSensor(true);
        
        b2CircleShape rightSensorShape;
        rightSensorShape.m_p.Set(0.2f, 0.0f);
        rightSensorShape.m_radius = wheelRadius_;
        rightSensorFixture_ = mainBody_->CreateFixture(&rightSensorShape, 0.0f);
        rightSensorFixture_->SetSensor(true);
    }

    bool MonsterPhysicsComponent::isStanding() const
    {
        for (b2ContactEdge const *edge = mainBody_->GetContactList(); edge;
             edge = edge->next)
        {
            if (edge->contact->IsTouching()) {
                b2Fixture const *fixtureA = edge->contact->GetFixtureA();
                b2Fixture const *fixtureB = edge->contact->GetFixtureB();
                if (fixtureA == bottomSensorFixture_ ||
                    fixtureB == bottomSensorFixture_)
                {
                    return true;
                }
            }
        }
        return false;
    }
}
