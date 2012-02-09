#include "actor_factory.hpp"

#include "block.hpp"
#include "chain.hpp"
#include "monster.hpp"

namespace crust {
    ActorFactory::ActorFactory(Game *game) :
        game_(game)
    { }

    std::auto_ptr<Block> ActorFactory::createBlock(Polygon2 const &polygon)
    {
        return std::auto_ptr<Block>(new Block(game_, polygon));
    }

    std::auto_ptr<Chain> ActorFactory::createChain(Vector2 const &position, int linkCount)
    {
        return std::auto_ptr<Chain>(new Chain(game_, position, linkCount));
    }

    std::auto_ptr<Monster> ActorFactory::createMonster(Vector2 const &position)
    {
        return std::auto_ptr<Monster>(new Monster(game_, position));
    }
}
