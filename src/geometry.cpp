#include "geometry.hpp"

namespace crust {
    // http://mathworld.wolfram.com/Circumcircle.html
    Circle2 Triangle2::getCircumcircle() const
    {
        float a = Matrix3(p1.x, p1.y, 1.0f,
                          p2.x, p2.y, 1.0f,
                          p3.x, p3.y, 1.0f).getDeterminant();
        float d1 = p1.getSquaredLength();
        float d2 = p2.getSquaredLength();
        float d3 = p3.getSquaredLength();
        float bX = -Matrix3(d1, p1.y, 1.0f,
                            d2, p2.y, 1.0f,
                            d3, p3.y, 1.0f).getDeterminant();
        float bY = Matrix3(d1, p1.x, 1.0f,
                           d2, p2.x, 1.0f,
                           d3, p3.x, 1.0f).getDeterminant();
        Vector2 b(bX, bY);
        float c = -Matrix3(d1, p1.x, p1.y,
                           d2, p2.x, p2.y,
                           d3, p3.x, p3.y).getDeterminant();
        float invA = 1.0f / a;
        Vector2 center = -0.5f * invA * b;
        float radius = (0.5f * std::abs(invA) *
                        std::sqrt(b.getSquaredLength() - 4.0f * a * c));
        return Circle2(center, radius);
    }
}
