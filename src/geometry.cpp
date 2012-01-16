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
}
