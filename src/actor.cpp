#include "actor.hpp"

#include "animation_component.hpp"
#include "control_component.hpp"
#include "physics_component.hpp"
#include "render_component.hpp"

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

    void Actor::setRenderComponent(std::auto_ptr<RenderComponent> component)
    {
        renderComponent_ = component;
    }

    void Actor::setAnimationComponent(std::auto_ptr<AnimationComponent> component)
    {
        animationComponent_ = component;
    }
}
