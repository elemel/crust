#ifndef CRUST_BLOCK_HPP
#define CRUST_BLOCK_HPP

#include "actor.hpp"

namespace crust {
    class Polygon2;

    class Block : public Actor {
    public:
        explicit Block(Game *game, Polygon2 const &polygon);
    };
}

#endif
