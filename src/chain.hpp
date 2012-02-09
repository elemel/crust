#ifndef CRUST_CHAIN_HPP
#define CRUST_CHAIN_HPP

#include "actor.hpp"

#include <vector>
#include <Box2D/Box2D.h>

namespace crust {
    class Game;
    class Vector2;
    
    class Chain : public Actor {
    public:
        Chain(Game *game, Vector2 const &position, int linkCount);
        ~Chain();

        Vector2 getPosition() const;
        void draw() const;

    private:
        std::vector<b2Body *> bodies_;
        b2RopeJoint *ropeJoint_;
    };
}

#endif
