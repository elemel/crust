#ifndef CRUST_BLOCK_HPP
#define CRUST_BLOCK_HPP

#include "actor.hpp"
#include "geometry.hpp"
#include "grid.hpp"

#include <vector>
#include <Box2D/Box2D.h>
#include <SDL/SDL_opengl.h>

namespace crust {
    class Box2;
    class Game;
    class Polygon2;
    class Vector2;
    
    class Block : public Actor {
    public:
        // With 4 bytes per float, adds up to 32 bytes.
        struct DrawVertex {
            GLfloat red;
            GLfloat green;
            GLfloat blue;

            GLfloat normalX;
            GLfloat normalY;
            GLfloat normalZ;

            GLfloat x;
            GLfloat y;
        };

        explicit Block(Game *game, Polygon2 const &polygon);
        ~Block(); 

        typedef std::pair<int, int> Key;

        b2Body *getPhysicsBody()
        {
            return body_;
        }

        Vector2 getPosition() const;
        void setPosition(float x, float y);
        void setAngle(float angle);
        void setColor(float red, float green, float blue);
        
        int getElement(int x, int y);
        void setElement(int x, int y, int type);

        bool findElementNearPosition(float x, float y);
        int getElementAtPosition(float x, float y);
        void setElementAtPosition(float x, float y, int type);
        
        void draw() const;

        Box2 getBounds() const;
        bool containsPoint(Vector2 const &point) const;

        Polygon2 const &getLocalPolygon() const
        {
            return localPolygon_;
        }
        
    private:
        Polygon2 localPolygon_;

        Grid<unsigned char> grid_;
        b2Body *body_;
        float red_;
        float green_;
        float blue_;
        float normalX_;
        float normalY_;

        mutable std::vector<DrawVertex> quadDrawVertices_;
        mutable std::vector<DrawVertex> lineDrawVertices_;
        mutable bool drawVerticesDirty_;

        float getColorOffset(int x, int y, int i) const;
        std::size_t hashValue(std::size_t a) const;
        void addGridPointToBounds(int x, int y, Box2 *bounds) const;

        void updateDrawVertices() const;
        void updateQuadDrawVertices() const;
        void updateLineDrawVertices() const;

        void rasterize(Polygon2 const &polygon);
    };
}

#endif
