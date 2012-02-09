#ifndef CRUST_PHYSICS_COMPONENT_HPP
#define CRUST_PHYSICS_COMPONENT_HPP

namespace crust {
    class PhysicsComponent {
    public:
        virtual ~PhysicsComponent()
        { }

        virtual Vector2 const &getPosition() const = 0;
        virtual float getAngle() const = 0;
    };
}

#endif
