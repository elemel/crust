#ifndef CRUST_ACTOR_HPP
#define CRUST_ACTOR_HPP

#include <memory>

namespace crust {
    class Component;
    class Game;
    
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
        
        Component *getPhysicsComponent()
        {
            return physicsComponent_.get();
        }
        
        Component const *getPhysicsComponent() const
        {
            return physicsComponent_.get();
        }
        
        void setPhysicsComponent(std::auto_ptr<Component> component);
        
        Component *getControlComponent()
        {
            return controlComponent_.get();
        }
        
        Component const *getControlComponent() const
        {
            return controlComponent_.get();
        }
        
        void setControlComponent(std::auto_ptr<Component> component);
        
        Component *getSceneComponent()
        {
            return sceneComponent_.get();
        }
        
        Component const *getSceneComponent() const
        {
            return sceneComponent_.get();
        }
        
        void setSceneComponent(std::auto_ptr<Component> component);
        
        Component *getAnimationComponent()
        {
            return animationComponent_.get();
        }
        
        Component const *getAnimationComponent() const
        {
            return animationComponent_.get();
        }
        
        void setAnimationComponent(std::auto_ptr<Component> component);

        void create();
        void destroy();
        
    private:
        Game *game_;
        
        std::auto_ptr<Component> physicsComponent_;
        std::auto_ptr<Component> controlComponent_;
        std::auto_ptr<Component> sceneComponent_;
        std::auto_ptr<Component> animationComponent_;
    };
}

#endif
