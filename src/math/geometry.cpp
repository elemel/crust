#include "geometry.hpp"

namespace crust {
    // http://mathworld.wolfram.com/Circumcircle.html
    Circle2 Triangle2::getCircumcircle() const
    {
        float a = Matrix3(v1.x, v1.y, 1.0f,
                          v2.x, v2.y, 1.0f,
                          v3.x, v3.y, 1.0f).getDeterminant();
        float d1 = v1.getSquaredLength();
        float d2 = v2.getSquaredLength();
        float d3 = v3.getSquaredLength();
        float bX = -Matrix3(d1, v1.y, 1.0f,
                            d2, v2.y, 1.0f,
                            d3, v3.y, 1.0f).getDeterminant();
        float bY = Matrix3(d1, v1.x, 1.0f,
                           d2, v2.x, 1.0f,
                           d3, v3.x, 1.0f).getDeterminant();
        Vector2 b(bX, bY);
        float c = -Matrix3(d1, v1.x, v1.y,
                           d2, v2.x, v2.y,
                           d3, v3.x, v3.y).getDeterminant();
        float invA = 1.0f / a;
        Vector2 center = -0.5f * invA * b;
        float radius = (0.5f * std::abs(invA) *
                        std::sqrt(b.getSquaredLength() - 4.0f * a * c));
        return Circle2(center, radius);
    }

    float Triangle2::getCircumcircleDeterminant(Vector2 const &p) const
    {
        float a = v1.x - p.x;
        float b = v1.y - p.y;
        float c = square(v1.x) - square(p.x) + square(v1.y) - square(p.y);
        float d = v2.x - p.x;
        float e = v2.y - p.y;
        float f = square(v2.x) - square(p.x) + square(v2.y) - square(p.y);
        float g = v3.x - p.x;
        float h = v3.y - p.y;
        float i = square(v3.x) - square(p.x) + square(v3.y) - square(p.y);
        return Matrix3(a, b, c, d, e, f, g, h, i).getDeterminant();
    }

    // http://en.wikipedia.org/wiki/Centroid
    float Polygon2::getArea() const
    {
        float area = 0.0f;
        for (std::size_t i = 0; i < vertices.size(); ++i) {
            Vector2 const &v1 = vertices[i];
            Vector2 const &v2 = vertices[(i + 1) % vertices.size()];
            area += v1.x * v2.y - v2.x * v1.y;
        }
        return 0.5f * area;
    }

    // http://en.wikipedia.org/wiki/Centroid
    Vector2 Polygon2::getCentroid() const
    {
        Vector2 centroid;
        for (std::size_t i = 0; i < vertices.size(); ++i) {
            Vector2 const &v1 = vertices[i];
            Vector2 const &v2 = vertices[(i + 1) % vertices.size()];
            centroid.x += (v1.x + v2.x) * (v1.x * v2.y - v2.x * v1.y);
            centroid.y += (v1.y + v2.y) * (v1.x * v2.y - v2.x * v1.y);
        }
        centroid /= 6.0f * getArea();
        return centroid;
    }

    Box2 Polygon2::getBoundingBox() const
    {
        Box2 bounds;
        for (std::size_t i = 0; i < vertices.size(); ++i) {
            bounds.mergePoint(vertices[i]);
        }
        return bounds;
    }

    bool Polygon2::containsPoint(Vector2 const &p) const
    {
        for (std::size_t i = 0; i < vertices.size(); ++i) {
            Vector2 const &v1 = vertices[i];
            Vector2 const &v2 = vertices[(i + 1) % vertices.size()];
            if (cross(p - v1, v2 - v1) > 0.0f) {
                return false;
            }
        }
        return true;
    }

    void Polygon2::pad(float f)
    {
        Vector2 centroid = getCentroid();
        for (std::size_t i = 0; i < vertices.size(); ++i) {
            Vector2 direction = vertices[i] - centroid;
            vertices[i] += f * normalize(direction);
        }
    }
    
    bool contains(Box2 const &outer, Polygon2 const &inner)
    {
        for (int i = 0; i < inner.getSize(); ++i) {
            if (!outer.containsPoint(inner.vertices[i])) {
                return false;
            }
        }
        return true;
    }
}
