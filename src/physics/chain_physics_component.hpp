#ifndef CRUST_CHAIN_PHYSICS_COMPONENT_HPP
#define CRUST_CHAIN_PHYSICS_COMPONENT_HPP

#include "geometry.hpp"
#include "physics_component.hpp"

#include <vector>
#include <Box2D/Box2D.h>

namespace crust {
    class Actor;
    class PhysicsService;

    class ChainPhysicsComponent : public PhysicsComponent {
    public:
        typedef std::vector<b2Body *> BodyVector;
        
        ChainPhysicsComponent(Actor *actor, Vector2 const &position, int linkCount);
        ~ChainPhysicsComponent();

        void create();
        void destroy();

        Vector2 getPosition() const;

        float getAngle() const
        {
            return 0.0f;
        }
        
        BodyVector const &getBodies() const
        {
            return bodies_;
        }

    private:
        Actor *actor_;
        PhysicsService *physicsService_;

        Vector2 position_;
        int linkCount_;
        
        BodyVector bodies_;
        b2RopeJoint *ropeJoint_;
    };
}

#endif
