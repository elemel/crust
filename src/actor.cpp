#include "actor.hpp"

#include "animation_component.hpp"
#include "control_component.hpp"
#include "physics_component.hpp"
#include "scene_component.hpp"

namespace crust {
    Actor::Actor(Game *game) :
        game_(game)
    { }

    Actor::~Actor()
    { }

    void Actor::setPhysicsComponent(std::auto_ptr<PhysicsComponent> component)
    {
        physicsComponent_ = component;
    }

    void Actor::setControlComponent(std::auto_ptr<ControlComponent> component)
    {
        controlComponent_ = component;
    }

    void Actor::setSceneComponent(std::auto_ptr<SceneComponent> component)
    {
        sceneComponent_ = component;
    }

    void Actor::setAnimationComponent(std::auto_ptr<AnimationComponent> component)
    {
        animationComponent_ = component;
    }

    void Actor::create()
    {
        if (physicsComponent_.get()) {
            physicsComponent_->create();
        }
        if (controlComponent_.get()) {
            controlComponent_->create();
        }
        if (sceneComponent_.get()) {
            sceneComponent_->create();
        }
        if (animationComponent_.get()) {
            animationComponent_->create();
        }
    }

    void Actor::destroy()
    {
        if (animationComponent_.get()) {
            animationComponent_->destroy();
        }
        if (sceneComponent_.get()) {
            sceneComponent_->destroy();
        }
        if (controlComponent_.get()) {
            controlComponent_->destroy();
        }
        if (physicsComponent_.get()) {
            physicsComponent_->destroy();
        }
    }
}
