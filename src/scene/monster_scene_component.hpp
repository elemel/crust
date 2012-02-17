#ifndef CRUST_MONSTER_SCENE_COMPONENT_HPP
#define CRUST_MONSTER_SCENE_COMPONENT_HPP

#include "scene_component.hpp"

#include <memory>

namespace crust {
    class Actor;
    class SceneService;
    class Sprite;

    class MonsterSceneComponent : public SceneComponent {
    public:
        explicit MonsterSceneComponent(Actor *actor);
        ~MonsterSceneComponent();

        void create();
        void destroy();

        Sprite *getHeadSprite()
        {
            return headSprite_.get();
        }

        Sprite *getTrunkSprite()
        {
            return trunkSprite_.get();
        }

    private:
        Actor *actor_;
        SceneService *sceneService_;

        std::auto_ptr<Sprite> headSprite_;
        std::auto_ptr<Sprite> trunkSprite_;

        void initSprites();
    };
}

#endif
