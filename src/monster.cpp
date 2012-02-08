#include "monster.hpp"

#include "debug_draw.hpp"
#include "game.hpp"
#include "geometry.hpp"

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
        jumpControl_(false)
    {
        initPhysics(position);
        initSprites();
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

    void Monster::stepPhysics(float dt)
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
            b2Vec2 velocity = body_->GetLinearVelocity();
            velocity.y = jumpVelocity_;
            body_->SetLinearVelocity(velocity);
            jumpTime_ = game_->getTime();
        }

        // Boost.
        if (!standing && jumpControl_ && 0.0f < velocity.y &&
            velocity.y < maxBoostVelocity_ &&
            game_->getTime() < jumpTime_ + boostDuration_)
        {
            velocity.y += boostAcceleration_ * dt;
            body_->SetLinearVelocity(velocity);
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

    void Monster::stepAnimation(float dt)
    {
        int xControl = int(rightControl_) - int(leftControl_);
        bool bodyTurned = false;
        if (xControl && xControl != bodyDirection_) {
            bodyDirection_ = xControl;
            bodySprite_.setScale(Vector2(0.1f * float(bodyDirection_), 0.1f));
            bodyTurned = true;
        }
        b2Vec2 localEyePosition = b2Vec2(0.0f, 0.35f);
        b2Vec2 localTargetPosition = body_->GetLocalPoint(b2Vec2(targetPosition_.x, targetPosition_.y));
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
        headSprite_.setAngle(headAngle);
        headSprite_.setScale(Vector2(0.1f * float(headDirection_), 0.1f));
    }

    void Monster::draw() const
    {
        glPushMatrix();
        b2Vec2 const &position = body_->GetPosition();
        float angle = body_->GetAngle();
        glTranslatef(position.x, position.y, 0.0f);
        glRotatef((180.0f / M_PI) * angle, 0.0f, 0.0f, 1.0f);
        bodySprite_.draw();
        headSprite_.draw();
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
    
    void Monster::initSprites()
    {
        headSprite_.setScale(Vector2(0.1f));
        bodySprite_.setScale(Vector2(0.1f));
        
        Color4 skinColor = parseColor4("#fc9");
        Color4 eyeColor = parseColor4("#000");
        Color4 noseColor = parseColor4("#f96");
        Color4 earColor = parseColor4("#f96");
        Color4 hairColor = parseColor4("#996");
        Color4 beardColor = parseColor4("#663");
        Color4 shirtColor = parseColor4("#9c3");
        Color4 beltColor = parseColor4("#630");
        Color4 buckleColor = parseColor4("#fc3");
        Color4 trouserColor = parseColor4("#390");
        Color4 bootColor = parseColor4("#960");
        
        headSprite_.setPixel(Vector2(-0.2f, 0.2f), skinColor);
        headSprite_.setPixel(Vector2(-0.1f, 0.2f), skinColor);
        headSprite_.setPixel(Vector2(0.0f, 0.2f), skinColor);
        headSprite_.setPixel(Vector2(0.1f, 0.2f), skinColor);
        headSprite_.setPixel(Vector2(0.2f, 0.2f), skinColor);
        headSprite_.setPixel(Vector2(0.3f, 0.2f), skinColor);
        
        headSprite_.setPixel(Vector2(-0.3f, 0.1f), skinColor);
        headSprite_.setPixel(Vector2(-0.2f, 0.1f), skinColor);
        headSprite_.setPixel(Vector2(-0.1f, 0.1f), skinColor);
        headSprite_.setPixel(Vector2(0.0f, 0.1f), skinColor);
        headSprite_.setPixel(Vector2(0.1f, 0.1f), skinColor);
        headSprite_.setPixel(Vector2(0.2f, 0.1f), skinColor);
        headSprite_.setPixel(Vector2(0.3f, 0.1f), skinColor);
        
        headSprite_.setPixel(Vector2(-0.3f, 0.0f), skinColor);
        headSprite_.setPixel(Vector2(-0.2f, 0.0f), skinColor);
        headSprite_.setPixel(Vector2(-0.1f, 0.0f), skinColor);
        headSprite_.setPixel(Vector2(0.0f, 0.0f), skinColor);
        headSprite_.setPixel(Vector2(0.1f, 0.0f), skinColor);
        headSprite_.setPixel(Vector2(0.2f, 0.0f), skinColor);
        headSprite_.setPixel(Vector2(0.3f, 0.0f), skinColor);
        
        headSprite_.setPixel(Vector2(-0.2f, -0.1f), skinColor);
        headSprite_.setPixel(Vector2(-0.1f, -0.1f), skinColor);
        headSprite_.setPixel(Vector2(0.0f, -0.1f), skinColor);
        headSprite_.setPixel(Vector2(0.1f, -0.1f), skinColor);
        headSprite_.setPixel(Vector2(0.2f, -0.1f), skinColor);

        headSprite_.setPixel(Vector2(-0.2f, 0.4f), hairColor);
        headSprite_.setPixel(Vector2(0.0f, 0.4f), hairColor);
        headSprite_.setPixel(Vector2(0.2f, 0.4f), hairColor);
        headSprite_.setPixel(Vector2(-0.3f, 0.3f), hairColor);
        headSprite_.setPixel(Vector2(-0.2f, 0.3f), hairColor);
        headSprite_.setPixel(Vector2(-0.1f, 0.3f), hairColor);
        headSprite_.setPixel(Vector2(0.0f, 0.3f), hairColor);
        headSprite_.setPixel(Vector2(0.1f, 0.3f), hairColor);
        headSprite_.setPixel(Vector2(0.2f, 0.3f), hairColor);
        headSprite_.setPixel(Vector2(-0.3f, 0.2f), hairColor);
        headSprite_.setPixel(Vector2(-0.2f, 0.2f), hairColor);
        headSprite_.setPixel(Vector2(-0.2f, 0.1f), hairColor);

        headSprite_.setPixel(Vector2(-0.2f, 0.0f), beardColor);
        headSprite_.setPixel(Vector2(-0.2f, -0.1f), beardColor);
        headSprite_.setPixel(Vector2(-0.1f, -0.1f), beardColor);
        headSprite_.setPixel(Vector2(0.0f, -0.1f), beardColor);
        headSprite_.setPixel(Vector2(0.1f, -0.1f), beardColor);
        headSprite_.setPixel(Vector2(0.2f, -0.1f), beardColor);
        headSprite_.setPixel(Vector2(0.3f, -0.1f), beardColor);
        headSprite_.setPixel(Vector2(-0.1f, -0.2f), beardColor);
        headSprite_.setPixel(Vector2(0.1f, -0.2f), beardColor);
        headSprite_.setPixel(Vector2(0.3f, -0.2f), beardColor);

        headSprite_.setPixel(Vector2(0.0f, 0.1f), eyeColor);
        headSprite_.setPixel(Vector2(0.2f, 0.1f), eyeColor);
        
        headSprite_.setPixel(Vector2(0.1f, 0.1f), noseColor);
        headSprite_.setPixel(Vector2(0.1f, 0.0f), noseColor);
        
        headSprite_.setPixel(Vector2(-0.3f, 0.1f), earColor);
        headSprite_.setPixel(Vector2(-0.3f, 0.0f), earColor);

        bodySprite_.setPixel(Vector2(-0.1f, 0.4f), skinColor);
        bodySprite_.setPixel(Vector2(0.0f, 0.4f), skinColor);
        bodySprite_.setPixel(Vector2(0.1f, 0.4f), skinColor);
        
        bodySprite_.setPixel(Vector2(-0.1f, 0.3f), skinColor);
        bodySprite_.setPixel(Vector2(0.0f, 0.3f), skinColor);
        bodySprite_.setPixel(Vector2(0.1f, 0.3f), skinColor);

        bodySprite_.setPixel(Vector2(-0.4f, 0.2f), shirtColor);
        bodySprite_.setPixel(Vector2(-0.3f, 0.2f), shirtColor);
        bodySprite_.setPixel(Vector2(-0.2f, 0.2f), shirtColor);
        bodySprite_.setPixel(Vector2(-0.1f, 0.2f), skinColor);
        bodySprite_.setPixel(Vector2(0.0f, 0.2f), skinColor);
        bodySprite_.setPixel(Vector2(0.1f, 0.2f), skinColor);
        bodySprite_.setPixel(Vector2(0.2f, 0.2f), shirtColor);
        bodySprite_.setPixel(Vector2(0.3f, 0.2f), shirtColor);
        bodySprite_.setPixel(Vector2(0.4f, 0.2f), shirtColor);
        
        bodySprite_.setPixel(Vector2(-0.5f, 0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(-0.4f, 0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(-0.3f, 0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(-0.2f, 0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(-0.1f, 0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(0.0f, 0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(0.1f, 0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(0.2f, 0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(0.3f, 0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(0.4f, 0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(0.5f, 0.1f), shirtColor);
        
        bodySprite_.setPixel(Vector2(-0.5f, 0.0f), shirtColor);
        bodySprite_.setPixel(Vector2(-0.3f, 0.0f), shirtColor);
        bodySprite_.setPixel(Vector2(-0.2f, 0.0f), shirtColor);
        bodySprite_.setPixel(Vector2(-0.1f, 0.0f), shirtColor);
        bodySprite_.setPixel(Vector2(0.0f, 0.0f), shirtColor);
        bodySprite_.setPixel(Vector2(0.1f, 0.0f), shirtColor);
        bodySprite_.setPixel(Vector2(0.2f, 0.0f), shirtColor);
        bodySprite_.setPixel(Vector2(0.3f, 0.0f), shirtColor);
        bodySprite_.setPixel(Vector2(0.5f, 0.0f), shirtColor);
        
        bodySprite_.setPixel(Vector2(-0.5f, -0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(-0.3f, -0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(-0.2f, -0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(-0.1f, -0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(0.0f, -0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(0.1f, -0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(0.2f, -0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(0.3f, -0.1f), shirtColor);
        bodySprite_.setPixel(Vector2(0.5f, -0.1f), shirtColor);
        
        bodySprite_.setPixel(Vector2(-0.3f, -0.2f), beltColor);
        bodySprite_.setPixel(Vector2(-0.2f, -0.2f), beltColor);
        bodySprite_.setPixel(Vector2(-0.1f, -0.2f), beltColor);
        bodySprite_.setPixel(Vector2(0.0f, -0.2f), beltColor);
        bodySprite_.setPixel(Vector2(0.1f, -0.2f), beltColor);
        bodySprite_.setPixel(Vector2(0.2f, -0.2f), beltColor);
        bodySprite_.setPixel(Vector2(0.3f, -0.2f), beltColor);

        bodySprite_.setPixel(Vector2(0.0f, -0.2f), buckleColor);
        bodySprite_.setPixel(Vector2(0.1f, -0.2f), buckleColor);

        bodySprite_.setPixel(Vector2(-0.5f, -0.2f), skinColor);
        bodySprite_.setPixel(Vector2(-0.5f, -0.3f), skinColor);

        bodySprite_.setPixel(Vector2(0.5f, -0.2f), skinColor);
        bodySprite_.setPixel(Vector2(0.5f, -0.3f), skinColor);

        bodySprite_.setPixel(Vector2(-0.3f, -0.3f), trouserColor);
        bodySprite_.setPixel(Vector2(-0.2f, -0.3f), trouserColor);
        bodySprite_.setPixel(Vector2(-0.1f, -0.3f), trouserColor);
        bodySprite_.setPixel(Vector2(0.0f, -0.3f), trouserColor);
        bodySprite_.setPixel(Vector2(0.1f, -0.3f), trouserColor);
        bodySprite_.setPixel(Vector2(0.2f, -0.3f), trouserColor);
        bodySprite_.setPixel(Vector2(0.3f, -0.3f), trouserColor);
        
        bodySprite_.setPixel(Vector2(-0.3f, -0.4f), trouserColor);
        bodySprite_.setPixel(Vector2(-0.2f, -0.4f), trouserColor);
        bodySprite_.setPixel(Vector2(0.2f, -0.4f), trouserColor);
        bodySprite_.setPixel(Vector2(0.3f, -0.4f), trouserColor);
        
        bodySprite_.setPixel(Vector2(-0.3f, -0.5f), bootColor);
        bodySprite_.setPixel(Vector2(-0.2f, -0.5f), bootColor);
        bodySprite_.setPixel(Vector2(0.2f, -0.5f), bootColor);
        bodySprite_.setPixel(Vector2(0.3f, -0.5f), bootColor);

        bodySprite_.setPixel(Vector2(-0.3f, -0.6f), bootColor);
        bodySprite_.setPixel(Vector2(-0.2f, -0.6f), bootColor);
        bodySprite_.setPixel(Vector2(-0.1f, -0.6f), bootColor);
        bodySprite_.setPixel(Vector2(0.2f, -0.6f), bootColor);
        bodySprite_.setPixel(Vector2(0.3f, -0.6f), bootColor);
        bodySprite_.setPixel(Vector2(0.4f, -0.6f), bootColor);
        
        headSprite_.setPosition(Vector2(0.0f, 0.25f));
        bodySprite_.setPosition(Vector2(0.0f, -0.15f));
    }
}
