#include "actor.hpp"

#include "component.hpp"

namespace crust {
    Actor::Actor(Game *game) :
        game_(game)
    { }

    Actor::~Actor()
    { }

    void Actor::setPhysicsComponent(std::auto_ptr<Component> component)
    {
        physicsComponent_ = component;
    }

    void Actor::setControlComponent(std::auto_ptr<Component> component)
    {
        controlComponent_ = component;
    }

    void Actor::setSceneComponent(std::auto_ptr<Component> component)
    {
        sceneComponent_ = component;
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
    }

    void Actor::destroy()
    {
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
