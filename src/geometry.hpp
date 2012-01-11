#ifndef CRUST_GEOMETRY_HPP
#define CRUST_GEOMETRY_HPP

#include "math.hpp"

#include <cmath>
#include <iostream>
#include <vector>

namespace crust {
    class Box2 {
    public:
        Vector2 p1;
        Vector2 p2;

        Box2() :
            p1(std::numeric_limits<float>::infinity(),
               std::numeric_limits<float>::infinity()),
            p2(-std::numeric_limits<float>::infinity(),
               -std::numeric_limits<float>::infinity())
        { }

        Box2(Vector2 const &p1, Vector2 const &p2) :
            p1(p1),
            p2(p2)
        { }

        Box2(float x, float y, float width, float height) :
            p1(x, y),
            p2(x + width, y + height)
        { }

        float getWidth() const
        {
            return (p1.x < p2.x) ? p2.x - p1.x : 0.0f;
        }
        
        float getHeight() const
        {
            return (p1.y < p2.y) ? p2.y - p1.y : 0.0f;
        }
        
        float getArea() const
        {
            return getWidth() * getHeight();
        }
        
        bool isEmpty() const
        {
            return p2.x < p1.x || p2.y < p1.y;
        }
        
        bool contains(float x, float y) const
        {
            return p1.x < x && x < p2.x && p1.y < y && y < p2.y;
        }

        void pad(float f)
        {
            p1.x -= f;
            p1.y -= f;
            p2.x += f;
            p2.y += f;
        }

        void pad(float x, float y)
        {
            p1.x -= x;
            p1.y -= y;
            p2.x += x;
            p2.y += y;
        }
        
        void merge(float x, float y)
        {
            p1.x = std::min(x, p1.x);
            p1.y = std::min(y, p1.y);
            p2.x = std::max(x, p2.x);
            p2.y = std::max(y, p2.y);
        }
    };

    class LineSegment2 {
    public:
        Vector2 p1, p2;

        LineSegment2()
        { }

        LineSegment2(Vector2 const &p1, Vector2 const &p2) :
            p1(p1),
            p2(p2)
        { }

        float getLength() const
        {
            return getDistance(p1, p2);
        }
        
        float getSquaredLength() const
        {
            return getSquaredDistance(p1, p2);
        }

        Box2 boundingBox() const;
    };

    inline Box2 LineSegment2::boundingBox() const
    {
        return Box2(Vector2(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
                    Vector2(std::max(p1.x, p2.x), std::max(p1.y, p2.y)));
    }

    class Circle2 {
    public:
        Vector2 center;
        float radius;

        Circle2();
        Circle2(Vector2 const &center, float radius);

        float perimeter() const;
        float area() const;
        Box2 boundingBox() const;

        void transform(Matrix3 const &m);
    };

    inline Circle2::Circle2() :
        radius(0.0f)
    { }

    inline Circle2::Circle2(Vector2 const &center, float radius) :
        center(center),
        radius(radius)
    { }

    inline float Circle2::perimeter() const
    {
        return 2.0f * pi * radius;
    }

    inline float Circle2::area() const
    {
        return pi * square(radius);
    }

    inline Box2 Circle2::boundingBox() const
    {
        return Box2(center - Vector2(radius), center + Vector2(radius));
    }

    inline Circle2 transform(Circle2 const &c, Matrix3 const &m)
    {
        Circle2 result(c);
        result.transform(m);
        return result;
    }

    class Polygon2 {
    public:
        std::vector<Vector2> vertices;

        Polygon2()
        { }

        explicit Polygon2(Box2 const &b);
        Polygon2(Circle2 const &c, int size, float angle = 0.0f);

        bool isEmpty() const
        {
            return vertices.empty();
        }

        int getSize() const
        {
            return vertices.size();
        }

        bool isClockwise() const
        {
            return (vertices.size() >= 3 &&
                    cross(vertices[1] - vertices[0],
                          vertices[2] - vertices[1]) <= 0.0f);
        }

        void reverse();
        void transform(Matrix3 const &m);
    };

    inline void Polygon2::reverse()
    {
        std::reverse(vertices.begin(), vertices.end());
    }

    inline Polygon2 transform(Polygon2 const &p, Matrix3 const &m)
    {
        Polygon2 result(p);
        result.transform(m);
        return result;
    }

    inline bool intersects(Vector2 const &p1, Vector2 const &p2)
    {
        (void) p1;
        (void) p2;
        return false;
    }

    inline bool intersects(Box2 const &b1, Box2 const &b2)
    {
        return (b1.p1.x <= b2.p2.x && b2.p1.x <= b1.p2.x &&
                b1.p1.y <= b2.p2.y && b2.p1.y <= b1.p2.y);
    }

    inline bool intersects(Box2 const &b, Vector2 const &p)
    {
        return (b.p1.x <= p.x && p.x <= b.p2.x &&
                b.p1.y <= p.y && p.y <= b.p2.y);
    }

    bool intersects(Box2 const &b, Circle2 const &c);

    inline bool intersects(Circle2 const &c1, Circle2 const &c2)
    {
        return (getSquaredDistance(c1.center, c2.center) <=
                square(c1.radius + c2.radius));
    }

    inline bool intersects(Circle2 const &c, Vector2 const &p)
    {
        return getSquaredDistance(c.center, p) <= square(c.radius);
    }

    inline bool intersects(Vector2 const &p, Box2 const &b)
    {
        return intersects(b, p);
    }

    inline bool intersects(Vector2 const &p, Circle2 const &c)
    {
        return intersects(c, p);
    }

    inline bool intersects(Circle2 const &c, Box2 const &b)
    {
        return intersects(b, c);
    }
}

#endif
