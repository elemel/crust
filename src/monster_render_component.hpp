#ifndef CRUST_MONSTER_RENDER_COMPONENT_HPP
#define CRUST_MONSTER_RENDER_COMPONENT_HPP

#include "render_component.hpp"

#include <memory>

namespace crust {
    class Monster;
    class Sprite;

    class MonsterRenderComponent : public RenderComponent {
    public:
        explicit MonsterRenderComponent(Monster *monster);

        Sprite *getHeadSprite()
        {
            return headSprite_.get();
        }

        Sprite *getBodySprite()
        {
            return bodySprite_.get();
        }

        void draw() const;

    private:
        Monster *monster_;

        std::auto_ptr<Sprite> headSprite_;
        std::auto_ptr<Sprite> bodySprite_;

        void initSprites();
    };
}

#endif
