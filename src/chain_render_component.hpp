#ifndef CRUST_CHAIN_RENDER_COMPONENT_HPP
#define CRUST_CHAIN_RENDER_COMPONENT_HPP

#include "render_component.hpp"

namespace crust {
    class Chain;

    class ChainRenderComponent : public RenderComponent {
    public:
        explicit ChainRenderComponent(Chain *chain);
        void draw() const;

    private:
        Chain *chain_;
    };
}

#endif
