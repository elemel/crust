#include "block.hpp"

#include "game.hpp"
#include "geometry.hpp"
#include "block_physics_component.hpp"
#include "block_render_component.hpp"

#include <cmath>
#include <iostream>
#include <boost/functional/hash.hpp>
#include <SDL/SDL_opengl.h>

namespace crust {
    Block::Block(Game *game, Polygon2 const &polygon) :
        Actor(game)
    {
        physicsComponent_.reset(new BlockPhysicsComponent(this, polygon));
        renderComponent_.reset(new BlockRenderComponent(this));
    }
}
