#include "physics_manager.hpp"

#include "physics_draw_callback.hpp"

namespace crust {
    PhysicsManager::PhysicsManager(Game *game) :
        game_(game)
    {
        b2Vec2 gravity(0.0f, -10.0f);
        world_.reset(new b2World(gravity));
        world_->SetContactListener(this);
        drawCallback_.reset(new PhysicsDrawCallback);
        world_->SetDebugDraw(drawCallback_.get());
    }

    PhysicsManager::~PhysicsManager()
    { }

    void PhysicsManager::step(float dt)
    {
        world_->Step(dt, 10, 10);
    }
}
