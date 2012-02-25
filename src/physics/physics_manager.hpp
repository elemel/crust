#ifndef CRUST_PHYSICS_MANAGER_HPP
#define CRUST_PHYSICS_MANAGER_HPP

#include <memory>
#include <Box2D/Box2D.h>

namespace crust {
    class Game;
    class PhysicsDrawCallback;

    class PhysicsManager : public b2ContactListener {
    public:
        explicit PhysicsManager(Game *game);
        ~PhysicsManager();

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
