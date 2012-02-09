#include "monster.hpp"

#include "animation_component.hpp"
#include "control_component.hpp"
#include "physics_component.hpp"
#include "render_component.hpp"

namespace crust {
    Monster::Monster(Game *game) :
        game_(game)
    { }

    Monster::~Monster()
    { }
}
