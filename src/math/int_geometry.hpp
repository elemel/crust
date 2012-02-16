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
        
        int getWidth() const
        {
            return (p1.x < p2.x) ? p2.x - p1.x : 0;
        }
        
        int getHeight() const
        {
            return (p1.y < p2.y) ? p2.y - p1.y : 0;
        }

        int getPerimeter() const
        {
            return 2 * (getWidth() + getHeight());
        }
        
        int getArea() const
        {
            return getWidth() * getHeight();
        }
        
        bool isEmpty() const
        {
            return p2.x <= p1.x || p2.y <= p1.y;
        }
        
        bool containsPoint(IntVector2 const &p) const
        {
            return p1.x <= p.x && p.x < p2.x && p1.y <= p.y && p.y < p2.y;
        }

        void pad(int i)
        {
            p1.x -= i;
            p1.y -= i;
            p2.x += i;
            p2.y += i;
        }

        void pad(IntVector2 const &p)
        {
            p1.x -= p.x;
            p1.y -= p.y;
            p2.x += p.x;
            p2.y += p.y;
        }
        
        void mergePoint(IntVector2 const &p)
        {
            p1.x = std::min(p.x, p1.x);
            p1.y = std::min(p.y, p1.y);
            p2.x = std::max(p.x + 1, p2.x);
            p2.y = std::max(p.y + 1, p2.y);
        }
    };
}

#endif
