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

        float getWidth() const
        {
            return p2.x - p1.x;
        }
        
        float getHeight() const
        {
            return p2.y - p1.y;
        }

        float getPerimeter() const
        {
            return 2.0f * (getWidth() + getHeight());
        }
        
        float getArea() const
        {
            return getWidth() * getHeight();
        }
        
        bool isEmpty() const
        {
            return p2.x < p1.x || p2.y < p1.y;
        }

        Vector2 getSize() const
        {
            return p2 - p1;
        }

        Vector2 getCenter() const
        {
            return 0.5f * (p1 + p2);
        }
        
        bool containsPoint(Vector2 const &p) const
        {
            return p1.x < p.x && p.x < p2.x && p1.y < p.y && p.y < p2.y;
        }

        void pad(float f)
        {
            p1.x -= f;
            p1.y -= f;
            p2.x += f;
            p2.y += f;
        }

        void pad(Vector2 const &p)
        {
            p1.x -= p.x;
            p1.y -= p.y;
            p2.x += p.x;
            p2.y += p.y;
        }
        
        void mergePoint(Vector2 const &p)
        {
            p1.x = std::min(p.x, p1.x);
            p1.y = std::min(p.y, p1.y);
            p2.x = std::max(p.x, p2.x);
            p2.y = std::max(p.y, p2.y);
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

        Box2 getBoundingBox() const
        {
            return Box2(Vector2(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
                        Vector2(std::max(p1.x, p2.x), std::max(p1.y, p2.y)));
        }
    };

    class Circle2 {
    public:
        Vector2 center;
        float radius;

        Circle2() :
            radius(0.0f)
        { }

        Circle2(Vector2 const &center, float radius) :
            center(center),
            radius(radius)
        { }

        float getPerimeter() const
        {
            return 2.0f * pi * radius;
        }

        float getArea() const
        {
            return pi * square(radius);
        }

        Box2 getBoundingBox() const
        {
            return Box2(center - Vector2(radius), center + Vector2(radius));
        }

        void transform(Matrix3 const &m)
        {
            Vector2 point = center + Vector2(radius, 0.0f);
            center = transformPoint(m, center);
            point = transformPoint(m, point);
            radius = getDistance(center, point);
        }
    };

    inline Circle2 transform(Circle2 const &c, Matrix3 const &m)
    {
        Circle2 result(c);
        result.transform(m);
        return result;
    }

    class Triangle2 {
    public:
        Vector2 v1;
        Vector2 v2;
        Vector2 v3;

        Triangle2()
        { }

        Triangle2(Vector2 const &v1, Vector2 const &v2, Vector2 const &v3) :
            v1(v1),
            v2(v2),
            v3(v3)
        { }

        Circle2 getCircumcircle() const;
        float getCircumcircleDeterminant(Vector2 const &p) const;
    };
    
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

        void reverse()
        {
            std::reverse(vertices.begin(), vertices.end());
        }

        void transform(Matrix3 const &m);

        float getArea() const;
        Vector2 getCentroid() const;
        Box2 getBoundingBox() const;

        bool containsPoint(Vector2 const &p) const;

        void pad(float f);
    };

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
        return (b1.p1.x < b2.p2.x && b2.p1.x < b1.p2.x &&
                b1.p1.y < b2.p2.y && b2.p1.y < b1.p2.y);
    }

    inline bool intersects(Box2 const &b, Vector2 const &p)
    {
        return (b.p1.x < p.x && p.x < b.p2.x &&
                b.p1.y < p.y && p.y < b.p2.y);
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

    inline bool contains(Box2 const &outer, Box2 const &inner)
    {
        return (outer.p1.x < inner.p1.x && inner.p2.x < outer.p2.x &&
                outer.p1.y < inner.p1.y && inner.p2.y < outer.p2.y);
    }
    
    bool contains(Box2 const &outer, Polygon2 const &inner);
}

#endif
