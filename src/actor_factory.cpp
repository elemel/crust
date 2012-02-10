#include "actor_factory.hpp"

#include "actor.hpp"
#include "block_physics_component.hpp"
#include "block_render_component.hpp"
#include "chain_physics_component.hpp"
#include "chain_render_component.hpp"
#include "monster_animation_component.hpp"
#include "monster_control_component.hpp"
#include "monster_physics_component.hpp"
#include "monster_render_component.hpp"

namespace crust {
    ActorFactory::ActorFactory(Game *game) :
        game_(game)
    { }

    std::auto_ptr<Actor> ActorFactory::createBlock(Polygon2 const &polygon)
    {
        std::auto_ptr<Actor> actor(new Actor(game_));
        actor->setPhysicsComponent(std::auto_ptr<PhysicsComponent>(new BlockPhysicsComponent(actor.get(), polygon)));
        actor->setRenderComponent(std::auto_ptr<RenderComponent>(new BlockRenderComponent(actor.get())));
        return actor;
    }

    std::auto_ptr<Actor> ActorFactory::createChain(Vector2 const &position, int linkCount)
    {
        std::auto_ptr<Actor> actor(new Actor(game_));
        actor->setPhysicsComponent(std::auto_ptr<PhysicsComponent>(new ChainPhysicsComponent(actor.get(), position, linkCount)));
        actor->setRenderComponent(std::auto_ptr<RenderComponent>(new ChainRenderComponent(actor.get())));
        return actor;
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
