#include "monster.hpp"

#include "debug_draw.hpp"
#include "game.hpp"
#include "geometry.hpp"
#include "ray_spring.hpp"

namespace crust {
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

        for (int i = 0; i < raySprings_.size(); ++i) {
            float angle = 5.0f * M_PI / 4.0f + 2.0f * M_PI / 4.0f * float(i) / float(raySprings_.size() - 1);
            b2Vec2 anchor1 = body_->GetPosition();
            b2Vec2 anchor2 = body_->GetWorldPoint(b2Vec2(std::cos(angle), std::sin(angle)));
            raySprings_.replace(i, new RaySpring(body_, anchor1, anchor2));
            raySprings_[i].setSpringConstant(100.0f);
            raySprings_[i].setDampingConstant(-20.0f);
            raySprings_[i].setMaxDampingForce(50.0f);
        }
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
        for (int i = 0; i < raySprings_.size(); ++i) {
            raySprings_[i].step(dt);
        }

        b2Vec2 moveForce(0.0f, 0.0f);
        b2Vec2 frictionForce(0.0f, 0.0f);
        int fixtureCount = 0;
        for (int i = 0; i < raySprings_.size(); ++i) {
            if (raySprings_[i].getFixture()) {
                b2Body *groundBody = raySprings_[i].getFixture()->GetBody();
                b2Vec2 groundVelocity = groundBody->GetLinearVelocityFromWorldPoint(raySprings_[i].getPoint());
                b2Vec2 bodyVelocity = body_->GetLinearVelocityFromWorldPoint(raySprings_[i].getPoint());
                b2Vec2 velocityDiff = bodyVelocity - groundVelocity;

                b2Vec2 moveDirection = b2Cross(1.0f, raySprings_[i].getDirection());
                moveForce += 25.0f * (float(rightControl_) - float(leftControl_)) * moveDirection;
                frictionForce += -5.0f * b2Dot(velocityDiff, moveDirection) * moveDirection;

                fixtureCount += 1;
            }
        }
        if (fixtureCount) {
            b2Vec2 force = (1.0f / float(fixtureCount)) * (moveForce + frictionForce);
            body_->ApplyForce(force, body_->GetPosition());
        }
    }

    void Monster::debugDraw() const
    {
        b2Vec2 position = body_->GetPosition();
        debugDrawCircle(position.x, position.y, 0.5f);
        for (int i = 0; i < raySprings_.size(); ++i) {
            raySprings_[i].debugDraw();
        }
    }
}
