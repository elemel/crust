#ifndef CRUST_COMPONENT_HPP
#define CRUST_COMPONENT_HPP

namespace crust {
    class Component {
    public:
        virtual ~Component()
        { }

        virtual void create() = 0;
        virtual void destroy() = 0;
    };
}

#endif
