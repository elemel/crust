#include "actor_factory.hpp"

#include "actor.hpp"
#include "block.hpp"
#include "chain.hpp"
#include "monster_animation_component.hpp"
#include "monster_control_component.hpp"
#include "monster_physics_component.hpp"
#include "monster_render_component.hpp"

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

    std::auto_ptr<Actor> ActorFactory::createMonster(Vector2 const &position)
    {
        std::auto_ptr<Actor> actor(new Actor(game_));
        actor->setPhysicsComponent(std::auto_ptr<PhysicsComponent>(new MonsterPhysicsComponent(actor.get(), position)));
        actor->setControlComponent(std::auto_ptr<ControlComponent>(new MonsterControlComponent(actor.get())));
        actor->setRenderComponent(std::auto_ptr<RenderComponent>(new MonsterRenderComponent(actor.get())));
        actor->setAnimationComponent(std::auto_ptr<AnimationComponent>(new MonsterAnimationComponent(actor.get())));
        return actor;
    }
}
