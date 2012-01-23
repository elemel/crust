#include "monster.hpp"

#include "game.hpp"
#include "geometry.hpp"

namespace crust {
    namespace {
        class MonsterRayCastCallback : public b2RayCastCallback {
        public:
            b2Fixture *fixture;
            b2Vec2 point;
            b2Vec2 normal;

            MonsterRayCastCallback() :
                fixture(0)
            { }
            
            float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
                                  const b2Vec2& normal, float32 fraction)
            {
                this->fixture = fixture;
                this->point = point;
                this->normal = normal;
                return fraction;
            }
        };
    }

    Monster::Monster(Game *game, Vector2 const &position) :
        game_(game),
        body_(0),
    
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
    }

    Monster::~Monster()
    {
        game_->getPhysicsWorld()->DestroyBody(body_);
    }

    Vector2 Monster::getPosition() const
    {
        b2Vec2 position = body_->GetPosition();
        return Vector2(position.x, position.y);
    }

    void Monster::step(float dt)
    {
        MonsterRayCastCallback callback;
        b2Vec2 p1 = body_->GetPosition();
        b2Vec2 p2 = p1 + b2Vec2(0.0f, -1.0f);
        game_->getPhysicsWorld()->RayCast(&callback, p1, p2);
        if (callback.fixture) {
            float springConst = 50.0f;
            float dampingConst = -10.0f;
            b2Vec2 positionDiff = callback.point - p2;
            b2Vec2 springForce = springConst * positionDiff;
            b2Body *groundBody = callback.fixture->GetBody();
            b2Vec2 groundVelocity = groundBody->GetLinearVelocityFromWorldPoint(callback.point);
            b2Vec2 bodyVelocity = body_->GetLinearVelocityFromWorldPoint(callback.point);
            b2Vec2 velocityDiff = bodyVelocity - groundVelocity;
            b2Vec2 dampingForce = dampingConst * velocityDiff;
            dampingForce.x = 0.0f;
            b2Vec2 groundForce = springForce + dampingForce;

            b2Vec2 walkForce = 25.0f * (float(rightControl_) - float(leftControl_)) * b2Vec2(1.0f, 0.0f);
            b2Vec2 brakeForce = -5.0f * velocityDiff;
            brakeForce.y = 0.0f;
            
            body_->ApplyForce(groundForce + walkForce + brakeForce, p1);

        
        }
    }
}
