#ifndef CRUST_INT_GEOMETRY_HPP
#define CRUST_INT_GEOMETRY_HPP

#include "int_math.hpp"

#include <algorithm>
#include <limits>

namespace crust {
    class IntBox2 {
    public:
        IntVector2 p1;
        IntVector2 p2;

        IntBox2() :
            p1(std::numeric_limits<int>::max(),
               std::numeric_limits<int>::max()),
            p2(std::numeric_limits<int>::min(),
               std::numeric_limits<int>::min())
        { }
        
        int getX() const
        {
            return p1.x;
        }
        
        int getY() const
        {
            return p1.y;
        }
        
        int getWidth() const
        {
            return (p1.x < p2.x) ? p2.x - p1.x : 0;
        }
        
        int getHeight() const
        {
            return (p1.y < p2.y) ? p2.y - p1.y : 0;
        }
        
        int getSize() const
        {
            return getWidth() * getHeight();
        }
        
        bool isEmpty() const
        {
            return p2.x <= p1.x || p2.y <= p1.y;
        }
        
        bool contains(int x, int y) const
        {
            return p1.x <= x && x < p2.x && p1.y <= y && y < p2.y;
        }
        
        void pad(int dx, int dy)
        {
            p1.x -= dx;
            p1.y -= dy;
            p2.x += dx;
            p2.y += dy;
        }
        
        void add(int x, int y)
        {
            p1.x = std::min(x, p1.x);
            p1.y = std::min(y, p1.y);
            p2.x = std::max(x + 1, p2.x);
            p2.y = std::max(y + 1, p2.y);
        }
    };
}

#endif
