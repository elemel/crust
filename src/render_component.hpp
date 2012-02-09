#ifndef CRUST_RENDER_COMPONENT_HPP
#define CRUST_RENDER_COMPONENT_HPP

namespace crust {
    class RenderComponent {
    public:
        virtual ~RenderComponent()
        { }

        virtual void draw() const = 0;
    };
}

#endif
