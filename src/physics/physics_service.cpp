#include "physics_service.hpp"

#include "physics_draw_callback.hpp"

namespace crust {
    PhysicsService::PhysicsService(Game *game) :
        game_(game)
    {
        b2Vec2 gravity(0.0f, -10.0f);
        world_.reset(new b2World(gravity));
        world_->SetContactListener(this);
        drawCallback_.reset(new PhysicsDrawCallback);
        world_->SetDebugDraw(drawCallback_.get());
    }

    PhysicsService::~PhysicsService()
    { }

    void PhysicsService::step(float dt)
    {
        world_->Step(dt, 10, 10);
    }
}
