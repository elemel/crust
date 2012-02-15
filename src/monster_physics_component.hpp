#ifndef CRUST_MONSTER_PHYSICS_COMPONENT_HPP
#define CRUST_MONSTER_PHYSICS_COMPONENT_HPP

#include "geometry.hpp"
#include "physics_component.hpp"

#include <memory>
#include <Box2D/Box2D.h>

namespace crust {
    class Actor;
    
    class MonsterPhysicsComponent : public PhysicsComponent {
    public:
        MonsterPhysicsComponent(Actor *actor, Vector2 const &position);
        ~MonsterPhysicsComponent();

        void create();
        void destroy();
        
        Vector2 getPosition() const;
        float getAngle() const;

        float getWheelRadius() const
        {
            return wheelRadius_;
        }
        
        b2Body *getMainBody()
        {
            return mainBody_;
        }

        b2Body *getWheelBody()
        {
            return wheelBody_;
        }

        b2RevoluteJoint *getWheelJoint()
        {
            return wheelJoint_;
        }

        bool isStanding() const;

    private:
        Actor *actor_;
        Vector2 position_;

        float wheelRadius_;
        
        b2Body *mainBody_;
        b2Body *wheelBody_;

        b2RevoluteJoint *wheelJoint_;

        b2Fixture *topSensorFixture_;
        b2Fixture *leftSensorFixture_;
        b2Fixture *bottomSensorFixture_;
        b2Fixture *rightSensorFixture_;
    };
}

#endif
