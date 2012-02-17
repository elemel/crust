#ifndef CRUST_PHYSICS_SERVICE_HPP
#define CRUST_PHYSICS_SERVICE_HPP

#include <memory>
#include <Box2D/Box2D.h>

namespace crust {
    class Game;
    class PhysicsDrawCallback;

    class PhysicsService : public b2ContactListener {
    public:
        explicit PhysicsService(Game *game);
        ~PhysicsService();

        void step(float dt);

        b2World *getWorld()
        {
            return world_.get();
        }

        b2World const *getWorld() const
        {
            return world_.get();
        }

        void BeginContact(b2Contact *contact)
        { }

        void EndContact(b2Contact *contact)
        { }

    private:
        Game *game_;
        std::auto_ptr<b2World> world_;
        std::auto_ptr<PhysicsDrawCallback> drawCallback_;
    };
}

#endif
