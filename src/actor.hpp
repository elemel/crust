#ifndef CRUST_ACTOR_HPP
#define CRUST_ACTOR_HPP

#include <memory>

namespace crust {
    class AnimationComponent;
    class ControlComponent;
    class Game;
    class PhysicsComponent;
    class SceneComponent;
    
    class Actor {
    public:
        explicit Actor(Game *game);
        ~Actor();
        
        Game *getGame()
        {
            return game_;
        }
        
        Game const *getGame() const
        {
            return game_;
        }
        
        PhysicsComponent *getPhysicsComponent()
        {
            return physicsComponent_.get();
        }
        
        PhysicsComponent const *getPhysicsComponent() const
        {
            return physicsComponent_.get();
        }
        
        void setPhysicsComponent(std::auto_ptr<PhysicsComponent> component);
        
        ControlComponent *getControlComponent()
        {
            return controlComponent_.get();
        }
        
        ControlComponent const *getControlComponent() const
        {
            return controlComponent_.get();
        }
        
        void setControlComponent(std::auto_ptr<ControlComponent> component);
        
        SceneComponent *getSceneComponent()
        {
            return sceneComponent_.get();
        }
        
        SceneComponent const *getSceneComponent() const
        {
            return sceneComponent_.get();
        }
        
        void setSceneComponent(std::auto_ptr<SceneComponent> component);
        
        AnimationComponent *getAnimationComponent()
        {
            return animationComponent_.get();
        }
        
        AnimationComponent const *getAnimationComponent() const
        {
            return animationComponent_.get();
        }
        
        void setAnimationComponent(std::auto_ptr<AnimationComponent> component);

        void create();
        void destroy();
        
    private:
        Game *game_;
        
        std::auto_ptr<PhysicsComponent> physicsComponent_;
        std::auto_ptr<ControlComponent> controlComponent_;
        std::auto_ptr<SceneComponent> sceneComponent_;
        std::auto_ptr<AnimationComponent> animationComponent_;
    };
}

#endif
