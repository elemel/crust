#ifndef CRUST_CONTROL_COMPONENT_HPP
#define CRUST_CONTROL_COMPONENT_HPP

namespace crust {
    class Vector2;

    class ControlComponent {
    public:
        virtual ~ControlComponent()
        { }

        virtual void create() = 0;
        virtual void destroy() = 0;
    };
}

#endif
