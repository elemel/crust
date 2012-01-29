#ifndef CRUST_SPRITE_HPP
#define CRUST_SPRITE_HPP

#include "color.hpp"
#include "geometry.hpp"
#include "grid.hpp"

namespace crust {
    class Sprite {
    public:
        explicit Sprite(Vector2 const &position = Vector2(),
                        float angle = 0.0f, float scale = 1.0f) :
            position_(position),
            angle_(angle),
            scale_(scale),

            pixels_(Color4(0, 0))
        { }

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

        void setPixel(Vector2 const &position, Color4 const &color);
        
        void draw() const;

    private:
        Vector2 position_;
        float angle_;
        Vector2 scale_;

        Grid<Color4> pixels_;
    };
}

#endif
