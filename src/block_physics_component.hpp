#ifndef CRUST_BLOCK_PHYSICS_COMPONENT_HPP
#define CRUST_BLOCK_PHYSICS_COMPONENT_HPP

#include "physics_component.hpp"

#include "geometry.hpp"
#include "grid.hpp"
#include <Box2D/Box2D.h>

namespace crust {
    class Actor;
    
    class BlockPhysicsComponent : public PhysicsComponent {
    public:
        explicit BlockPhysicsComponent(Actor *actor, Polygon2 const &polygon);
        ~BlockPhysicsComponent();

        b2Body *getBody()
        {
            return body_;
        }
        
        Vector2 getPosition() const;
        void setPosition(float x, float y);

        float getAngle() const;
        void setAngle(float angle);
        
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

    private:
        Actor *actor_;

        Polygon2 localPolygon_;
        
        Grid<unsigned char> grid_;
        b2Body *body_;
        
        void rasterize(Polygon2 const &polygon);
        
        void addGridPointToBounds(int x, int y, Box2 *bounds) const;
    };
}

#endif
