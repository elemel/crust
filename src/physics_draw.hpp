#ifndef CRUST_PHYSICS_DRAW_HPP
#define CRUST_PHYSICS_DRAW_HPP

#include <memory>
#include <vector>
#include <Box2D/Box2D.h>

namespace crust {
    class PhysicsDraw : public b2Draw {
    public:
        PhysicsDraw();
        
        void DrawPolygon(b2Vec2 const *vertices, int32 vertexCount,
                         b2Color const &color);
        void DrawSolidPolygon(b2Vec2 const *vertices, int32 vertexCount,
                              b2Color const &color);
        void DrawCircle(b2Vec2 const &center, float32 radius,
                        b2Color const &color);
        void DrawSolidCircle(b2Vec2 const &center, float32 radius,
                             b2Vec2 const &axis, b2Color const &color);
        void DrawSegment(b2Vec2 const &p1, b2Vec2 const &p2,
                         b2Color const &color);
        void DrawTransform(b2Transform const &transform);
        
    private:
        b2Transform transform_;
        std::vector<b2Vec2> circleVertices_;
    };
}

#endif
