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
}
