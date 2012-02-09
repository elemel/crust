#ifndef CRUST_ANIMATION_COMPONENT_HPP
#define CRUST_ANIMATION_COMPONENT_HPP

namespace crust {
    class AnimationComponent {
    public:
        virtual ~AnimationComponent()
        { }

        virtual void step(float dt) = 0;
    };
}

#endif
