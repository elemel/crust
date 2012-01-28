#include "monster.hpp"

#include "debug_draw.hpp"
#include "game.hpp"
#include "geometry.hpp"
#include "ray_spring.hpp"

namespace crust {
    Monster::Monster(Game *game, Vector2 const &position) :
        game_(game),
        body_(0),
        wheelBody_(0),
        wheelJoint_(0),
        topSensorFixture_(0),
        leftSensorFixture_(0),
        bottomSensorFixture_(0),
        rightSensorFixture_(0),

        wheelRadius_(0.4f),
        maxVelocity_(5.0f),
        jumpImpulse_(6.0f),
        jumpDuration_(0.2f),
        maxDriftVelocity_(3.0f),
        driftForce_(10.0f),
        maxBoostVelocity_(7.0f),
        boostDuration_(0.3f),
        boostForce_(15.0f),
        jumpTime_(0.0f),
        faceDirection_(1),

        leftControl_(false),
        rightControl_(false),
        jumpControl_(false)
    {
        initPhysics(position);
        initSprite();
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
        if (xControl) {
            faceDirection_ = xControl;
        }
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

    void Monster::draw() const
    {
        glPushMatrix();
        b2Vec2 const &position = body_->GetPosition();
        float angle = body_->GetAngle();
        glTranslatef(position.x, position.y, 0.0f);
        glRotatef((180.0f / M_PI) * angle, 0.0f, 0.0f, 1.0f);
        if (faceDirection_ == -1) {
            glScalef(-1.0f, 1.0f, 1.0f);
        }
        sprite_.draw();
        glPopMatrix();
    }
    
    bool Monster::isStanding()
    {
        for (b2ContactEdge *edge = body_->GetContactList(); edge;
             edge = edge->next)
        {
            if (edge->contact->IsTouching()) {
                b2Fixture *fixtureA = edge->contact->GetFixtureA();
                b2Fixture *fixtureB = edge->contact->GetFixtureB();
                if (fixtureA == bottomSensorFixture_ ||
                    fixtureB == bottomSensorFixture_)
                {
                    return true;
                }
            }
        }
        return false;
    }

    void Monster::initPhysics(Vector2 const &position)
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
        wheelBodyDef.position.Set(position.x, position.y - 0.4f);
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
        
        b2CircleShape topSensorShape;
        topSensorShape.m_p.Set(0.0f, 0.2f);
        topSensorShape.m_radius = wheelRadius_;
        topSensorFixture_ = body_->CreateFixture(&topSensorShape, 0.0f);
        topSensorFixture_->SetSensor(true);
        
        b2CircleShape leftSensorShape;
        leftSensorShape.m_p.Set(-0.2f, 0.0f);
        leftSensorShape.m_radius = wheelRadius_;
        leftSensorFixture_ = body_->CreateFixture(&leftSensorShape, 0.0f);
        leftSensorFixture_->SetSensor(true);
        
        b2CircleShape bottomSensorShape;
        bottomSensorShape.m_p.Set(0.0f, -0.5f);
        bottomSensorShape.m_radius = wheelRadius_;
        bottomSensorFixture_ = body_->CreateFixture(&bottomSensorShape, 0.0f);
        bottomSensorFixture_->SetSensor(true);
        
        b2CircleShape rightSensorShape;
        rightSensorShape.m_p.Set(0.2f, 0.0f);
        rightSensorShape.m_radius = wheelRadius_;
        rightSensorFixture_ = body_->CreateFixture(&rightSensorShape, 0.0f);
        rightSensorFixture_->SetSensor(true);
    }
    
    void Monster::initSprite()
    {
        sprite_.setScale(0.1f);
        
        Color4 skinColor = parseColor4("#fc9");
        Color4 eyeColor = parseColor4("#000");
        Color4 noseColor = parseColor4("#c96");
        Color4 earColor = parseColor4("#c96");
        Color4 shirtColor = parseColor4("#f30");
        Color4 leggingsColor = parseColor4("#930");
        Color4 bootsColor = parseColor4("#960");
        
        sprite_.setPixel(Vector2(-0.2f, 0.6f), skinColor);
        sprite_.setPixel(Vector2(-0.1f, 0.6f), skinColor);
        sprite_.setPixel(Vector2(0.0f, 0.6f), skinColor);
        sprite_.setPixel(Vector2(0.1f, 0.6f), skinColor);
        sprite_.setPixel(Vector2(0.2f, 0.6f), skinColor);
        
        sprite_.setPixel(Vector2(-0.3f, 0.5f), skinColor);
        sprite_.setPixel(Vector2(-0.2f, 0.5f), skinColor);
        sprite_.setPixel(Vector2(-0.1f, 0.5f), skinColor);
        sprite_.setPixel(Vector2(0.0f, 0.5f), skinColor);
        sprite_.setPixel(Vector2(0.1f, 0.5f), skinColor);
        sprite_.setPixel(Vector2(0.2f, 0.5f), skinColor);
        sprite_.setPixel(Vector2(0.3f, 0.5f), skinColor);
        
        sprite_.setPixel(Vector2(-0.3f, 0.4f), skinColor);
        sprite_.setPixel(Vector2(-0.2f, 0.4f), skinColor);
        sprite_.setPixel(Vector2(-0.1f, 0.4f), skinColor);
        sprite_.setPixel(Vector2(0.0f, 0.4f), skinColor);
        sprite_.setPixel(Vector2(0.1f, 0.4f), skinColor);
        sprite_.setPixel(Vector2(0.2f, 0.4f), skinColor);
        sprite_.setPixel(Vector2(0.3f, 0.4f), skinColor);
        
        sprite_.setPixel(Vector2(-0.2f, 0.3f), skinColor);
        sprite_.setPixel(Vector2(-0.1f, 0.3f), skinColor);
        sprite_.setPixel(Vector2(0.0f, 0.3f), skinColor);
        sprite_.setPixel(Vector2(0.1f, 0.3f), skinColor);
        sprite_.setPixel(Vector2(0.2f, 0.3f), skinColor);
        
        sprite_.setPixel(Vector2(0.0f, 0.5f), eyeColor);
        sprite_.setPixel(Vector2(0.2f, 0.5f), eyeColor);
        
        sprite_.setPixel(Vector2(0.1f, 0.5f), noseColor);
        sprite_.setPixel(Vector2(0.1f, 0.4f), noseColor);
        
        sprite_.setPixel(Vector2(-0.2f, 0.5f), earColor);
        sprite_.setPixel(Vector2(-0.2f, 0.4f), earColor);
        
        sprite_.setPixel(Vector2(-0.4f, 0.2f), shirtColor);
        sprite_.setPixel(Vector2(-0.3f, 0.2f), shirtColor);
        sprite_.setPixel(Vector2(-0.2f, 0.2f), shirtColor);
        sprite_.setPixel(Vector2(-0.1f, 0.2f), shirtColor);
        sprite_.setPixel(Vector2(0.0f, 0.2f), shirtColor);
        sprite_.setPixel(Vector2(0.1f, 0.2f), shirtColor);
        sprite_.setPixel(Vector2(0.2f, 0.2f), shirtColor);
        sprite_.setPixel(Vector2(0.3f, 0.2f), shirtColor);
        sprite_.setPixel(Vector2(0.4f, 0.2f), shirtColor);
        
        sprite_.setPixel(Vector2(-0.5f, 0.1f), shirtColor);
        sprite_.setPixel(Vector2(-0.4f, 0.1f), shirtColor);
        sprite_.setPixel(Vector2(-0.3f, 0.1f), shirtColor);
        sprite_.setPixel(Vector2(-0.2f, 0.1f), shirtColor);
        sprite_.setPixel(Vector2(-0.1f, 0.1f), shirtColor);
        sprite_.setPixel(Vector2(0.0f, 0.1f), shirtColor);
        sprite_.setPixel(Vector2(0.1f, 0.1f), shirtColor);
        sprite_.setPixel(Vector2(0.2f, 0.1f), shirtColor);
        sprite_.setPixel(Vector2(0.3f, 0.1f), shirtColor);
        sprite_.setPixel(Vector2(0.4f, 0.1f), shirtColor);
        sprite_.setPixel(Vector2(0.5f, 0.1f), shirtColor);
        
        sprite_.setPixel(Vector2(-0.5f, 0.0f), shirtColor);
        sprite_.setPixel(Vector2(-0.3f, 0.0f), shirtColor);
        sprite_.setPixel(Vector2(-0.2f, 0.0f), shirtColor);
        sprite_.setPixel(Vector2(-0.1f, 0.0f), shirtColor);
        sprite_.setPixel(Vector2(0.0f, 0.0f), shirtColor);
        sprite_.setPixel(Vector2(0.1f, 0.0f), shirtColor);
        sprite_.setPixel(Vector2(0.2f, 0.0f), shirtColor);
        sprite_.setPixel(Vector2(0.3f, 0.0f), shirtColor);
        sprite_.setPixel(Vector2(0.5f, 0.0f), shirtColor);
        
        sprite_.setPixel(Vector2(-0.5f, -0.1f), shirtColor);
        sprite_.setPixel(Vector2(-0.3f, -0.1f), shirtColor);
        sprite_.setPixel(Vector2(-0.2f, -0.1f), shirtColor);
        sprite_.setPixel(Vector2(-0.1f, -0.1f), shirtColor);
        sprite_.setPixel(Vector2(0.0f, -0.1f), shirtColor);
        sprite_.setPixel(Vector2(0.1f, -0.1f), shirtColor);
        sprite_.setPixel(Vector2(0.2f, -0.1f), shirtColor);
        sprite_.setPixel(Vector2(0.3f, -0.1f), shirtColor);
        sprite_.setPixel(Vector2(0.5f, -0.1f), shirtColor);
        
        sprite_.setPixel(Vector2(-0.5f, -0.2f), skinColor);
        sprite_.setPixel(Vector2(-0.3f, -0.2f), shirtColor);
        sprite_.setPixel(Vector2(-0.2f, -0.2f), shirtColor);
        sprite_.setPixel(Vector2(-0.1f, -0.2f), shirtColor);
        sprite_.setPixel(Vector2(0.0f, -0.2f), shirtColor);
        sprite_.setPixel(Vector2(0.1f, -0.2f), shirtColor);
        sprite_.setPixel(Vector2(0.2f, -0.2f), shirtColor);
        sprite_.setPixel(Vector2(0.3f, -0.2f), shirtColor);
        sprite_.setPixel(Vector2(0.5f, -0.2f), skinColor);
        
        sprite_.setPixel(Vector2(-0.5f, -0.3f), skinColor);
        sprite_.setPixel(Vector2(-0.3f, -0.3f), leggingsColor);
        sprite_.setPixel(Vector2(-0.2f, -0.3f), leggingsColor);
        sprite_.setPixel(Vector2(-0.1f, -0.3f), leggingsColor);
        sprite_.setPixel(Vector2(0.0f, -0.3f), leggingsColor);
        sprite_.setPixel(Vector2(0.1f, -0.3f), leggingsColor);
        sprite_.setPixel(Vector2(0.2f, -0.3f), leggingsColor);
        sprite_.setPixel(Vector2(0.3f, -0.3f), leggingsColor);
        sprite_.setPixel(Vector2(0.5f, -0.3f), skinColor);
        
        sprite_.setPixel(Vector2(-0.3f, -0.4f), leggingsColor);
        sprite_.setPixel(Vector2(-0.2f, -0.4f), leggingsColor);
        sprite_.setPixel(Vector2(0.2f, -0.4f), leggingsColor);
        sprite_.setPixel(Vector2(0.3f, -0.4f), leggingsColor);
        
        sprite_.setPixel(Vector2(-0.3f, -0.5f), bootsColor);
        sprite_.setPixel(Vector2(-0.2f, -0.5f), bootsColor);
        sprite_.setPixel(Vector2(0.2f, -0.5f), bootsColor);
        sprite_.setPixel(Vector2(0.3f, -0.5f), bootsColor);
        
        sprite_.setPixel(Vector2(-0.3f, -0.6f), bootsColor);
        sprite_.setPixel(Vector2(-0.2f, -0.6f), bootsColor);
        sprite_.setPixel(Vector2(-0.1f, -0.6f), bootsColor);
        sprite_.setPixel(Vector2(0.2f, -0.6f), bootsColor);
        sprite_.setPixel(Vector2(0.3f, -0.6f), bootsColor);
        sprite_.setPixel(Vector2(0.4f, -0.6f), bootsColor);
        
        sprite_.setPosition(Vector2(0.0f, -0.15f));
    }
}
