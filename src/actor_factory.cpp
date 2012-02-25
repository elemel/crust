#include "actor_factory.hpp"

#include "actor.hpp"
#include "block_graphics_component.hpp"
#include "block_physics_component.hpp"
#include "component.hpp"
#include "monster_control_component.hpp"
#include "monster_graphics_component.hpp"
#include "monster_physics_component.hpp"

namespace crust {
    ActorFactory::ActorFactory(Game *game) :
        game_(game)
    { }

    std::auto_ptr<Actor> ActorFactory::createBlock(Polygon2 const &polygon)
    {
        std::auto_ptr<Actor> actor(new Actor(game_));
        actor->setPhysicsComponent(std::auto_ptr<Component>(new BlockPhysicsComponent(actor.get(), polygon)));
        actor->setGraphicsComponent(std::auto_ptr<Component>(new BlockGraphicsComponent(actor.get())));
        return actor;
    }

    std::auto_ptr<Actor> ActorFactory::createMonster(Vector2 const &position)
    {
        std::auto_ptr<Actor> actor(new Actor(game_));
        actor->setPhysicsComponent(std::auto_ptr<Component>(new MonsterPhysicsComponent(actor.get(), position)));
        actor->setControlComponent(std::auto_ptr<Component>(new MonsterControlComponent(actor.get())));
        actor->setGraphicsComponent(std::auto_ptr<Component>(new MonsterGraphicsComponent(actor.get())));
        return actor;
    }
}
