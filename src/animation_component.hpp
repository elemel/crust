#ifndef CRUST_ANIMATION_COMPONENT_HPP
#define CRUST_ANIMATION_COMPONENT_HPP

namespace crust {
    class AnimationComponent {
    public:
        virtual ~AnimationComponent()
        { }

        virtual void create() = 0;
        virtual void destroy() = 0;

        virtual void step(float dt) = 0;
    };
}

#endif
