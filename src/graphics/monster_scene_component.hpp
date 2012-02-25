#ifndef CRUST_MONSTER_SCENE_COMPONENT_HPP
#define CRUST_MONSTER_SCENE_COMPONENT_HPP

#include "component.hpp"
#include "task.hpp"

#include <memory>

namespace crust {
    class Actor;
    class MonsterControlComponent;
    class MonsterPhysicsComponent;
    class SceneService;
    class Sprite;

    class MonsterSceneComponent : public Component, public Task {
    public:
        explicit MonsterSceneComponent(Actor *actor);
        ~MonsterSceneComponent();

        void create();
        void destroy();

        void step(float dt);

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
        MonsterControlComponent *controlComponent_;
        MonsterPhysicsComponent *physicsComponent_;
        SceneService *sceneService_;

        int headDirection_;
        int trunkDirection_;

        std::auto_ptr<Sprite> headSprite_;
        std::auto_ptr<Sprite> trunkSprite_;

        void initSprites();
    };
}

#endif
