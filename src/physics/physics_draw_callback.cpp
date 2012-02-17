#include "physics_draw_callback.hpp"

#include <SDL/SDL_opengl.h>

namespace crust {
    PhysicsDrawCallback::PhysicsDrawCallback()
    {
        transform_.SetIdentity();
        SetFlags(e_shapeBit);
        int vertexCount = 32;
        for (int i = 0; i < vertexCount; ++i) {
            float angle = 2.0f * M_PI * float(i) / float(vertexCount);
            b2Vec2 vec(std::cos(angle), std::sin(angle));
            circleVertices_.push_back(vec);
        }
    }
        
    void PhysicsDrawCallback::DrawPolygon(b2Vec2 const *vertices,
                                          int32 vertexCount,
                                          b2Color const &color)
    {
        glBegin(GL_LINE_LOOP);
        for (int32 i = 0; i < vertexCount; ++i) {
            b2Vec2 transformedVertex = b2Mul(transform_, vertices[i]);
            glVertex2f(transformedVertex.x, transformedVertex.y);
        }
        glEnd();
    }
        
    void PhysicsDrawCallback::DrawSolidPolygon(b2Vec2 const *vertices,
                                               int32 vertexCount,
                                               b2Color const &color)
    {
        DrawPolygon(vertices, vertexCount, color);
    }
    
    void PhysicsDrawCallback::DrawCircle(b2Vec2 const &center, float32 radius,
                                         b2Color const &color)
    {
        glBegin(GL_LINE_LOOP);
        for (std::size_t i = 0; i < circleVertices_.size(); ++i) {
            b2Vec2 vertex = center + radius * circleVertices_[i];
            b2Vec2 transformedVertex = b2Mul(transform_, vertex);
            glVertex2f(transformedVertex.x, transformedVertex.y);
        }
        glEnd();
    }
        
    void PhysicsDrawCallback::DrawSolidCircle(b2Vec2 const &center,
                                              float32 radius,
                                              b2Vec2 const &axis,
                                              b2Color const &color)
    {
        DrawCircle(center, radius, color);
        DrawSegment(center, center + radius * axis, color);
    }
    
    void PhysicsDrawCallback::DrawSegment(b2Vec2 const &p1, b2Vec2 const &p2,
                                          b2Color const &color)
    {
        b2Vec2 transformedP1 = b2Mul(transform_, p1);
        b2Vec2 transformedP2 = b2Mul(transform_, p2);
        glBegin(GL_LINES);
        glVertex2f(transformedP1.x, transformedP1.y);
        glVertex2f(transformedP2.x, transformedP2.y);
        glEnd();
    }
    
    void PhysicsDrawCallback::DrawTransform(b2Transform const &transform)
    {
        transform_ = transform;
    }
}
