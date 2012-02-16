#ifndef CRUST_RENDER_COMPONENT_HPP
#define CRUST_RENDER_COMPONENT_HPP

namespace crust {
    class RenderComponent {
    public:
        virtual ~RenderComponent()
        { }

        virtual void create() = 0;
        virtual void destroy() = 0;

        virtual void draw() const = 0;
    };
}

#endif
