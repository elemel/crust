#ifndef CRUST_BOX_HPP
#define CRUST_BOX_HPP

#include <algorithm>
#include <limits>

namespace crust {
    class Box {
    public:
        Box() :
            minX_(std::numeric_limits<float>::infinity()),
            minY_(std::numeric_limits<float>::infinity()),
            maxX_(-std::numeric_limits<float>::infinity()),
            maxY_(-std::numeric_limits<float>::infinity())
        { }

        Box(float x, float y, float width, float height) :
            minX_(x),
            minY_(y),
            maxX_(x + width),
            maxY_(y + height)
        { }

        float getX() const
        {
            return minX_;
        }
        
        float getY() const
        {
            return minY_;
        }
        
        float getWidth() const
        {
            return (minX_ < maxX_) ? maxX_ - minX_ : 0.0f;
        }
        
        float getHeight() const
        {
            return (minY_ < maxY_) ? maxY_ - minY_ : 0.0f;
        }
        
        float getSize() const
        {
            return getWidth() * getHeight();
        }
        
        bool isEmpty() const
        {
            return maxX_ < minX_ || maxY_ < minY_;
        }
        
        bool contains(float x, float y) const
        {
            return minX_ < x && x < maxX_ && minY_ < y && y < maxY_;
        }
        
        void pad(float dx, float dy)
        {
            minX_ -= dx;
            minY_ -= dy;
            maxX_ += dx;
            maxY_ += dy;
        }
        
        void add(float x, float y)
        {
            minX_ = std::min(x, minX_);
            minY_ = std::min(y, minY_);
            maxX_ = std::max(x, maxX_);
            maxY_ = std::max(y, maxY_);
        }

        friend bool intersects(Box const &a, Box const &b)
        {
            return (a.minX_ < b.maxX_ && b.minX_ < a.maxX_ &&
                    a.minY_ < b.maxY_ && b.minY_ < a.maxY_);
        }

    private:
        float minX_;
        float minY_;
        float maxX_;
        float maxY_;
    };
}

#endif
