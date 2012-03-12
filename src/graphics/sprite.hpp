#ifndef CRUST_SPRITE_HPP
#define CRUST_SPRITE_HPP

#include "color.hpp"
#include "geometry.hpp"
#include "grid.hpp"
#include "int_geometry.hpp"
#include "texture.hpp"

#include <SDL/SDL_opengl.h>

namespace crust {
    class Sprite {
    public:
        Sprite();

        IntVector2 const &getSize() const
        {
            return size_;
        }

        Vector2 const &getPosition() const
        {
            return position_;
        }

        void setPosition(Vector2 const &position)
        {
            position_ = position;
            arraysDirty_ = true;
        }

        float getAngle() const
        {
            return angle_;
        }
        
        void setAngle(float angle)
        {
            angle_ = angle;
            arraysDirty_ = true;
        }

        Vector2 const &getScale() const
        {
            return scale_;
        }
        
        void setScale(Vector2 const &scale)
        {
            scale_ = scale;
            arraysDirty_ = true;
        }

        Color4 const &getColor() const
        {
            return color_;
        }

        void setColor(Color4 const &color)
        {
            color_ = color;
            arraysDirty_ = true;
        }

        Vector2 const &getAnchor() const
        {
            return anchor_;
        }
        
        void setAnchor(Vector2 const &anchor)
        {
            anchor_ = anchor;
            arraysDirty_ = true;
        }

        void setPixel(int x, int y, Color4 const &color)
        {
            pixels_.setElement(x, y, color);
            size_.x = pixels_.getWidth() + 4;
            size_.y = pixels_.getHeight() + 4;
            texturesDirty_ = true;
            arraysDirty_ = true;
        }
        
        void draw() const;

    private:
        IntVector2 size_;
        Vector2 position_;
        float angle_;
        Vector2 scale_;
        Color4 color_;
        Vector2 anchor_;

        Grid<Color4> pixels_;

        mutable bool texturesDirty_;
        mutable Texture colorTexture_;
        mutable Texture normalAndShadowTexture_;

        mutable bool arraysDirty_;
        mutable GLfloat vertexArray_[8];
        mutable GLfloat texCoordArray_[8];
        mutable GLubyte colorArray_[16];

        void updateTextures() const;
        void updateColorTexture() const;
        void updateNormalAndShadowTexture() const;

        void updateArrays() const;
    };
}

#endif
