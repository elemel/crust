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
        typedef std::vector<b2Body *> BodyVector;

        Chain(Game *game, Vector2 const &position, int linkCount);
        ~Chain();

        Vector2 getPosition() const;

        BodyVector const &getBodies() const
        {
            return bodies_;
        }
        
    private:
        BodyVector bodies_;
        b2RopeJoint *ropeJoint_;
    };
}

#endif
