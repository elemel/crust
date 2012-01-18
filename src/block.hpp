#ifndef CRUST_BLOCK_HPP
#define CRUST_BLOCK_HPP

#include "grid.hpp"

#include <vector>
#include <Box2D/Box2D.h>
#include <SDL/SDL_opengl.h>

namespace crust {
    class Box2;
    class Game;
    class Polygon2;
    
    class Block {
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

        explicit Block(Game *game, float x, float y, float angle);
        ~Block(); 

        typedef std::pair<int, int> Key;

        void setPosition(float x, float y);
        void setAngle(float angle);
        void setColor(float red, float green, float blue);
        
        int getElement(int x, int y);
        void setElement(int x, int y, int type);

        bool findElementNearPosition(float x, float y);
        int getElementAtPosition(float x, float y);
        void setElementAtPosition(float x, float y, int type);
        
        void absorbElement(int type);
        int emitElement();
        
        void draw();

        Box2 getBounds();
        int getNeighborCount();

        void dig(Box2 const &box);

        void fitPhysicsShapes();

        void rasterize(Polygon2 const &polygon);

    private:
        Game *game_;
        Grid<unsigned char> grid_;
        b2Body *body_;
        float red_;
        float green_;
        float blue_;
        float normalX_;
        float normalY_;
        std::vector<std::pair<int, int> > neighbors_;
        std::vector<DrawVertex> drawVertices_;
        bool drawVerticesDirty_;

        float getColorOffset(int x, int y, int i);
        std::size_t hashValue(std::size_t a);
        Block *findOtherBlock(int x, int y);
        void addGridPointToBounds(int x, int y, Box2 *bounds);
        void updateDrawVertices();
        void getNormal(int x, int y, float *normalX, float *normalY);
    };
}

#endif
