#ifndef CRUST_SCENE_COMPONENT_HPP
#define CRUST_SCENE_COMPONENT_HPP

namespace crust {
    class SceneComponent {
    public:
        virtual ~SceneComponent()
        { }

        virtual void create() = 0;
        virtual void destroy() = 0;

        virtual void draw() const = 0;
    };
}

#endif
