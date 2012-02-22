#ifndef CRUST_BLOCK_PHYSICS_COMPONENT_HPP
#define CRUST_BLOCK_PHYSICS_COMPONENT_HPP

#include "component.hpp"

#include "geometry.hpp"
#include "grid.hpp"
#include <Box2D/Box2D.h>

namespace crust {
    class Actor;
    class PhysicsService;
    
    class BlockPhysicsComponent : public Component {
    public:
        explicit BlockPhysicsComponent(Actor *actor, Polygon2 const &polygon);
        ~BlockPhysicsComponent();

        b2Body *getBody()
        {
            return body_;
        }

        void create();
        void destroy();

        int getElement(int x, int y);
        void setElement(int x, int y, int type);
        
        bool findElementNearPosition(float x, float y);
        int getElementAtPosition(float x, float y);
        void setElementAtPosition(float x, float y, int type);
        
        Box2 getBounds() const;
        bool containsPoint(Vector2 const &point) const;
        
        Polygon2 const &getLocalPolygon() const
        {
            return localPolygon_;
        }
        
        Grid<unsigned char> const &getGrid() const
        {
            return grid_;
        }

        float getMineDuration() const
        {
            return mineDuration_;
        }
        
        void setMineDuration(float duration)
        {
            mineDuration_ = duration;
        }
        
    private:
        Actor *actor_;
        PhysicsService *physicsService_;

        Polygon2 polygon_;

        Polygon2 localPolygon_;
        
        Grid<unsigned char> grid_;
        b2Body *body_;

        float mineDuration_;
        
        void rasterize(Polygon2 const &polygon);
        
        void addGridPointToBounds(int x, int y, Box2 *bounds) const;
    };
}

#endif
