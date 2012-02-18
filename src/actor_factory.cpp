#include "actor_factory.hpp"

#include "actor.hpp"
#include "block_physics_component.hpp"
#include "block_scene_component.hpp"
#include "chain_physics_component.hpp"
#include "chain_scene_component.hpp"
#include "component.hpp"
#include "monster_animation_component.hpp"
#include "monster_control_component.hpp"
#include "monster_physics_component.hpp"
#include "monster_scene_component.hpp"

namespace crust {
    ActorFactory::ActorFactory(Game *game) :
        game_(game)
    { }

    std::auto_ptr<Actor> ActorFactory::createBlock(Polygon2 const &polygon)
    {
        std::auto_ptr<Actor> actor(new Actor(game_));
        actor->setPhysicsComponent(std::auto_ptr<Component>(new BlockPhysicsComponent(actor.get(), polygon)));
        actor->setSceneComponent(std::auto_ptr<Component>(new BlockSceneComponent(actor.get())));
        return actor;
    }

    std::auto_ptr<Actor> ActorFactory::createChain(Vector2 const &position, int linkCount)
    {
        std::auto_ptr<Actor> actor(new Actor(game_));
        actor->setPhysicsComponent(std::auto_ptr<Component>(new ChainPhysicsComponent(actor.get(), position, linkCount)));
        actor->setSceneComponent(std::auto_ptr<Component>(new ChainSceneComponent(actor.get())));
        return actor;
    }

    std::auto_ptr<Actor> ActorFactory::createMonster(Vector2 const &position)
    {
        std::auto_ptr<Actor> actor(new Actor(game_));
        actor->setPhysicsComponent(std::auto_ptr<Component>(new MonsterPhysicsComponent(actor.get(), position)));
        actor->setControlComponent(std::auto_ptr<Component>(new MonsterControlComponent(actor.get())));
        actor->setSceneComponent(std::auto_ptr<Component>(new MonsterSceneComponent(actor.get())));
        actor->setAnimationComponent(std::auto_ptr<Component>(new MonsterAnimationComponent(actor.get())));
        return actor;
    }
}
