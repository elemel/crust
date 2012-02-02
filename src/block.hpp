#ifndef CRUST_BLOCK_HPP
#define CRUST_BLOCK_HPP

#include "actor.hpp"
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
        
        void draw();

        Box2 getBounds();

        void makeDynamic()
        {
            body_->SetType(b2_dynamicBody);
        }

        void makeStaticIfSleeping()
        {
            if (body_->GetType() != b2_staticBody && !body_->IsAwake()) {
                body_->SetType(b2_staticBody);
            }
        }
        
    private:
        Game *game_;
        Grid<unsigned char> grid_;
        b2Body *body_;
        float red_;
        float green_;
        float blue_;
        float normalX_;
        float normalY_;
        std::vector<DrawVertex> quadDrawVertices_;
        std::vector<DrawVertex> lineDrawVertices_;
        bool drawVerticesDirty_;

        float getColorOffset(int x, int y, int i);
        std::size_t hashValue(std::size_t a);
        void addGridPointToBounds(int x, int y, Box2 *bounds);

        void updateDrawVertices();
        void updateQuadDrawVertices();
        void updateLineDrawVertices();

        void rasterize(Polygon2 const &polygon);
    };
}

#endif
