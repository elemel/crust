#ifndef CRUST_GRID_BOX_HPP
#define CRUST_GRID_BOX_HPP

#include <algorithm>
#include <limits>

namespace crust {
    class GridBox {
    public:
        GridBox() :
            minX_(std::numeric_limits<int>::max()),
            minY_(std::numeric_limits<int>::max()),
            maxX_(std::numeric_limits<int>::min()),
            maxY_(std::numeric_limits<int>::min())
        { }
        
        int getX() const
        {
            return minX_;
        }
        
        int getY() const
        {
            return minY_;
        }
        
        int getWidth() const
        {
            return (minX_ < maxX_) ? maxX_ - minX_ : 0;
        }
        
        int getHeight() const
        {
            return (minY_ < maxY_) ? maxY_ - minY_ : 0;
        }
        
        int getSize() const
        {
            return getWidth() * getHeight();
        }
        
        bool isEmpty() const
        {
            return maxX_ <= minX_ || maxY_ <= minY_;
        }
        
        bool contains(int x, int y) const
        {
            return minX_ <= x && x < maxX_ && minY_ <= y && y < maxY_;
        }
        
        void pad(int dx, int dy)
        {
            minX_ -= dx;
            minY_ -= dy;
            maxX_ += dx;
            maxY_ += dy;
        }
        
        void add(int x, int y)
        {
            minX_ = std::min(x, minX_);
            minY_ = std::min(y, minY_);
            maxX_ = std::max(x + 1, maxX_);
            maxY_ = std::max(y + 1, maxY_);
        }
        
    private:
        int minX_;
        int minY_;
        int maxX_;
        int maxY_;
    };
}

#endif
