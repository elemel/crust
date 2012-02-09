#ifndef CRUST_MONSTER_HPP
#define CRUST_MONSTER_HPP

#include "actor.hpp"

namespace crust {
    class Monster : public Actor {
    public:
        explicit Monster(Game *game) :
            Actor(game)
        { }
    };
}

#endif
