#ifndef CRUST_RAY_SPRING_HPP
#define CRUST_RAY_SPRING_HPP

#include "geometry.hpp"

#include <Box2D/Box2D.h>

namespace crust {
    class RaySpring {
    public:
        RaySpring(b2Body *body, b2Vec2 const &anchor1, b2Vec2 const &anchor2);

        void setSpringConstant(float constant)
        {
            springConstant_ = constant;
        }

        void setDampingConstant(float constant)
        {
            dampingConstant_ = constant;
        }

        void setMaxDampingForce(float force)
        {
            maxDampingForce_ = force;
        }

        void step(float dt);

        b2Fixture *getFixture() const
        {
            return fixture_;
        }

        b2Vec2 const &getPoint() const
        {
            return point_;
        }

        b2Vec2 const &getNormal() const
        {
            return normal_;
        }

        b2Vec2 getDirection() const;
        
        void debugDraw() const;

    private:
        b2Body *body_;
        b2Vec2 localAnchor1_;
        b2Vec2 localAnchor2_;
        
        float springConstant_;
        float dampingConstant_;
        float maxDampingForce_;

        b2Fixture *fixture_;
        b2Vec2 point_;
        b2Vec2 normal_;
    };
}

#endif
