#ifndef CRUST_PHYSICS_COMPONENT_HPP
#define CRUST_PHYSICS_COMPONENT_HPP

namespace crust {
    class Vector2;

    class PhysicsComponent {
    public:
        virtual ~PhysicsComponent()
        { }

        virtual void create() = 0;
        virtual void destroy() = 0;
    };
}

#endif
