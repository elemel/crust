#include "ray_spring.hpp"

#include "debug_draw.hpp"

#include <SDL/SDL_opengl.h>

namespace crust {
    namespace {
        class RaySpringCallback : public b2RayCastCallback {
        public:
            b2Fixture *fixture;
            b2Vec2 point;
            b2Vec2 normal;
            
            RaySpringCallback() :
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
    
    RaySpring::RaySpring(b2Body *body, b2Vec2 const &anchor1,
                         b2Vec2 const &anchor2) :
        body_(body),
        localAnchor1_(body->GetLocalPoint(anchor1)),
        localAnchor2_(body->GetLocalPoint(anchor2)),
        springConstant_(1.0f),
        dampingConstant_(1.0f),
        maxDampingForce_(1.0f),
        fixture_(0)
    { }

    void RaySpring::step(float dt)
    {
        RaySpringCallback callback;
        b2Vec2 anchor1 = body_->GetWorldPoint(localAnchor1_);
        b2Vec2 anchor2 = body_->GetWorldPoint(localAnchor2_);
        body_->GetWorld()->RayCast(&callback, anchor1, anchor2);

        fixture_ = callback.fixture;
        point_ = callback.point;
        normal_ = callback.normal;
        
        if (callback.fixture) {
            b2Vec2 positionDiff = callback.point - anchor2;
            b2Vec2 springForce = springConstant_ * positionDiff;
            
            b2Body *groundBody = callback.fixture->GetBody();
            b2Vec2 groundVelocity = groundBody->GetLinearVelocityFromWorldPoint(callback.point);
            b2Vec2 bodyVelocity = body_->GetLinearVelocityFromWorldPoint(callback.point);
            b2Vec2 velocityDiff = bodyVelocity - groundVelocity;
            b2Vec2 dampingForce = dampingConstant_ * velocityDiff;
            float dampingForceLength = dampingForce.Normalize();
            dampingForce *= std::min(dampingForceLength, maxDampingForce_);
            
            b2Vec2 force = springForce + dampingForce;
            b2Vec2 direction = anchor2 - anchor1;
            direction.Normalize();
            force = std::min(b2Dot(force, direction), 0.0f) * direction;
            body_->ApplyForce(force, anchor1);
            groundBody->ApplyForce(-force, anchor2);
        }
    }

    b2Vec2 RaySpring::getDirection() const
    {
        b2Vec2 anchor1 = body_->GetWorldPoint(localAnchor1_);
        b2Vec2 anchor2 = body_->GetWorldPoint(localAnchor2_);
        b2Vec2 direction = anchor2 - anchor1;
        direction.Normalize();
        return direction;
    }
    
    void RaySpring::debugDraw() const
    {
        b2Vec2 anchor1 = body_->GetWorldPoint(localAnchor1_);
        b2Vec2 anchor2 = body_->GetWorldPoint(localAnchor2_);
        debugDrawLine(anchor1.x, anchor1.y, anchor2.x, anchor2.y);
    }
}
