#ifndef CRUST_SPRITE_HPP
#define CRUST_SPRITE_HPP

#include "color.hpp"
#include "geometry.hpp"
#include "grid.hpp"

#include <SDL/SDL_opengl.h>

namespace crust {
    class Sprite {
    public:
        struct Vertex {
            GLfloat red;
            GLfloat green;
            GLfloat blue;
            
            GLfloat normalX;
            GLfloat normalY;
            GLfloat normalZ;
            
            GLfloat x;
            GLfloat y;
        };

        Sprite();
        ~Sprite();
        
        Vector2 const &getPosition() const
        {
            return position_;
        }

        void setPosition(Vector2 const &position)
        {
            position_ = position;
        }

        float getAngle() const
        {
            return angle_;
        }
        
        void setAngle(float angle)
        {
            angle_ = angle;
        }

        Vector2 const &getScale() const
        {
            return scale_;
        }
        
        void setScale(Vector2 const &scale)
        {
            scale_ = scale;
        }

        Color4 const &getColor() const
        {
            return color_;
        }

        void setColor(Color4 const &color)
        {
            color_ = color;
        }
        
        void setPixel(int x, int y, Color4 const &color)
        {
            pixels_.setElement(x, y, color);
            verticesDirty_ = true;
            bufferDirty_ = true;
            textureDirty_ = true;
        }
        
        void draw() const;

    private:
        Vector2 position_;
        float angle_;
        Vector2 scale_;
        Color4 color_;

        Grid<Color4> pixels_;

        mutable bool verticesDirty_;
        mutable std::vector<Vertex> vertices_;

        mutable bool bufferDirty_;
        mutable GLuint bufferName_;
        mutable GLsizei bufferCount_;

        mutable bool textureDirty_;
        mutable GLuint texture_;
        mutable GLuint shadowTexture_;
        
        void drawDirectMode() const;
        void drawVertices() const;
        void drawBuffer() const;
        void drawTexture() const;

        void updateVertices() const;
        void updateBuffer() const;
        void updateTexture() const;
    };
}

#endif
