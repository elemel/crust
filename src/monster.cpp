#include "monster.hpp"

#include "debug_draw.hpp"
#include "game.hpp"
#include "geometry.hpp"
#include "monster_animation_component.hpp"
#include "monster_control_component.hpp"
#include "monster_physics_component.hpp"
#include "monster_render_component.hpp"

namespace crust {
    Monster::Monster(Game *game, Vector2 const &position) :
        game_(game)
    {
        physicsComponent_.reset(new MonsterPhysicsComponent(this, position));
        controlComponent_.reset(new MonsterControlComponent(this));
        renderComponent_.reset(new MonsterRenderComponent(this));
        animationComponent_.reset(new MonsterAnimationComponent(this));
    }

    Monster::~Monster()
    { }
}
