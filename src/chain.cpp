#include "chain.hpp"

#include "chain_physics_component.hpp"
#include "chain_render_component.hpp"
#include "game.hpp"

namespace crust {
    Chain::Chain(Game *game, Vector2 const &position, int linkCount) :
        Actor(game)
    {
        physicsComponent_.reset(new ChainPhysicsComponent(this, position, linkCount));
        renderComponent_.reset(new ChainRenderComponent(this));
    }
}