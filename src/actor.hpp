#ifndef CRUST_ACTOR_HPP
#define CRUST_ACTOR_HPP

#include <memory>

namespace crust {
    class AnimationComponent;
    class ControlComponent;
    class Game;
    class PhysicsComponent;
    class RenderComponent;
    
    class Actor {
    public:
        explicit Actor(Game *game);

        // TODO: Make non-virtual once all subclasses are gone.
        virtual ~Actor();
        
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
        
        void setPhysicsComponent(std::auto_ptr<PhysicsComponent> component)
        {
            physicsComponent_ = component;
        }
        
        ControlComponent *getControlComponent()
        {
            return controlComponent_.get();
        }
        
        ControlComponent const *getControlComponent() const
        {
            return controlComponent_.get();
        }
        
        void setControlComponent(std::auto_ptr<ControlComponent> component)
        {
            controlComponent_ = component;
        }
        
        RenderComponent *getRenderComponent()
        {
            return renderComponent_.get();
        }
        
        RenderComponent const *getRenderComponent() const
        {
            return renderComponent_.get();
        }
        
        void setRenderComponent(std::auto_ptr<RenderComponent> component)
        {
            renderComponent_ = component;
        }
        
        AnimationComponent *getAnimationComponent()
        {
            return animationComponent_.get();
        }
        
        AnimationComponent const *getAnimationComponent() const
        {
            return animationComponent_.get();
        }
        
        void setAnimationComponent(std::auto_ptr<AnimationComponent> component)
        {
            animationComponent_ = component;
        }
        
    // TODO: Make private once all subclasses are gone.
    protected:
        Game *game_;
        
        std::auto_ptr<PhysicsComponent> physicsComponent_;
        std::auto_ptr<ControlComponent> controlComponent_;
        std::auto_ptr<RenderComponent> renderComponent_;
        std::auto_ptr<AnimationComponent> animationComponent_;
    };
}

#endif
