#ifndef CRUST_CHAIN_HPP
#define CRUST_CHAIN_HPP

#include "actor.hpp"

namespace crust {
    class Game;
    class Vector2;
    
    class Chain : public Actor {
    public:
        Chain(Game *game, Vector2 const &position, int linkCount);
    };
}

#endif
