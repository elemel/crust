#ifndef CRUST_ACTOR_FACTORY_HPP
#define CRUST_ACTOR_FACTORY_HPP

#include <memory>

namespace crust {
    class Block;
    class Chain;
    class Game;
    class Monster;
    class Polygon2;
    class Vector2;

    class ActorFactory {
    public:
        explicit ActorFactory(Game *game);

        std::auto_ptr<Block> createBlock(Polygon2 const &polygon);
        std::auto_ptr<Chain> createChain(Vector2 const &position, int linkCount);
        std::auto_ptr<Monster> createMonster(Vector2 const &position);

    private:
        Game *game_;
    };
}

#endif
