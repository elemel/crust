#ifndef CRUST_MATH_HPP
#define CRUST_MATH_HPP

#include <cmath>
#include <iostream>
#include <vector>

namespace crust {
    const float pi = 3.14159265358979323846264338327950288419716939937510f;

    inline bool finite(float f)
    {
        return std::abs(f) <= std::numeric_limits<float>::max();
    }

    inline float sign(float f)
    {
        return f < 0.0f ? -1.0f : 1.0f;
    }

    inline float square(float f)
    {
        return f * f;
    }

    inline float clamp(float f, float minValue, float maxValue)
    {
        return std::min(std::max(f, minValue), maxValue);
    }

    class Vector2 {
    public:
        float x, y;

        Vector2() :
            x(0.0f), y(0.0f)
        { }
        
        explicit Vector2(float f) :
            x(f), y(f)
        { }
        
        Vector2(float x, float y) :
            x(x), y(y)
        { }
        
        float getLength() const
        {
            return std::sqrt(getSquaredLength());
        }
        
        float getSquaredLength() const
        {
            return square(x) + square(y);
        }
        
        Vector2 operator-() const
        {
            return Vector2(-x, -y);
        }
        
        Vector2 &operator+=(Vector2 const &v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }
        
        Vector2 &operator-=(Vector2 const &v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }
        
        Vector2 &operator*=(float f)
        {
            x *= f;
            y *= f;
            return *this;
        }
        
        Vector2 &operator/=(float f)
        {
            x /= f;
            y /= f;
            return *this;
        }
        
        float normalize()
        {
            float f = getSquaredLength();
            if (f != 0.0f && f != 1.0f) {
                f = std::sqrt(f);
                *this /= f;
            }
            return f;
        }
        
        void clampLength(float maxLength)
        {
            if (getSquaredLength() > square(maxLength)) {
                normalize();
                *this *= maxLength;
            }
        }
    };

    inline Vector2 operator+(Vector2 const &v1, Vector2 const &v2)
    {
        return Vector2(v1) += v2;
    }

    inline Vector2 operator-(Vector2 const &v1, Vector2 const &v2)
    {
        return Vector2(v1) -= v2;
    }

    inline Vector2 operator*(Vector2 const &v, float f)
    {
        return Vector2(v) *= f;
    }

    inline Vector2 operator*(float f, Vector2 const &v)
    {
        return Vector2(v) *= f;
    }

    inline Vector2 operator/(Vector2 const &v, float f)
    {
        return Vector2(v) /= f;
    }

    inline float dot(Vector2 const &v1, Vector2 const &v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }

    inline float cross(Vector2 const &v1, Vector2 const &v2)
    {
        return v1.x * v2.y - v2.x * v1.y;
    }

    inline Vector2 cross(Vector2 const &v)
    {
        return Vector2(-v.y, v.x);
    }

    inline Vector2 normalize(Vector2 const &v)
    {
        Vector2 result(v);
        result.normalize();
        return result;
    }

    inline Vector2 clampLength(Vector2 const &v, float maxLength)
    {
        Vector2 result(v);
        result.clampLength(maxLength);
        return result;
    }

    inline float getDistance(Vector2 const &p1, Vector2 const &p2)
    {
        return (p2 - p1).getLength();
    }

    inline float getSquaredDistance(Vector2 const &p1, Vector2 const &p2)
    {
        return (p2 - p1).getSquaredLength();
    }

    class Matrix3 {
    public:
        float a, b, c;
        float d, e, f;
        float g, h, i;

        Matrix3() :
            a(1.0f), b(0.0f), c(0.0f),
            d(0.0f), e(1.0f), f(0.0f),
            g(0.0f), h(0.0f), i(1.0f)
        { }

        explicit Matrix3(float x) :
            a(x), b(x), c(x),
            d(x), e(x), f(x),
            g(x), h(x), i(x)
        { }

        Matrix3(float a, float b, float c,
                float d, float e, float f) :
            a(a), b(b), c(c),
            d(d), e(e), f(f),
            g(0.0f), h(0.0f), i(1.0f)
        { }

        Matrix3(float a, float b, float c,
                float d, float e, float f,
                float g, float h, float i) :
            a(a), b(b), c(c),
            d(d), e(e), f(f),
            g(g), h(h), i(i)
        { }

        Matrix3 &operator+=(Matrix3 const &m)
        {
            a += m.a;
            b += m.b;
            c += m.c;
            
            d += m.d;
            e += m.e;
            f += m.f;
            
            g += m.g;
            h += m.h;
            i += m.i;
            
            return *this;
        }
        
        Matrix3 &operator-=(Matrix3 const &m)
        {
            a -= m.a;
            b -= m.b;
            c -= m.c;
            
            d -= m.d;
            e -= m.e;
            f -= m.f;
            
            g -= m.g;
            h -= m.h;
            i -= m.i;
            
            return *this;
        }

        Matrix3 &operator*=(Matrix3 const &m);

        Matrix3 &operator*=(float x)
        {
            a *= x;
            b *= x;
            c *= x;
            
            d *= x;
            e *= x;
            f *= x;
            
            g *= x;
            h *= x;
            i *= x;
            
            return *this;
        }
        
        Matrix3 &operator/=(float x)
        {
            return *this *= 1.0f / x;
        }
        
        void transpose()
        {
            std::swap(d, b);
            std::swap(g, c);
            std::swap(h, f);
        }

        float getDeterminant() const
        {
            return (a * e * i - a * f * h -
                    b * d * i + b * f * g +
                    c * d * h - c * e * g);
        }
        
        void invert()
        {
            float det = getDeterminant();
            float invDet = 1.0f / det;
            
            float a2 = e * i - f * h;
            float b2 = c * h - b * i;
            float c2 = b * f - c * e;

            float d2 = f * g - d * i;
            float e2 = a * i - c * g;
            float f2 = c * d - a * f;

            float g2 = d * h - e * g;
            float h2 = b * g - a * h;
            float i2 = a * e - b * d;

            a = invDet * a2;
            b = invDet * b2;
            c = invDet * c2;

            d = invDet * d2;
            e = invDet * e2;
            f = invDet * f2;

            g = invDet * g2;
            h = invDet * h2;
            i = invDet * i2;
        }

        void translate(Vector2 const &v)
        {
            *this *= Matrix3(1.0f, 0.0f, v.x, 0.0f, 1.0f, v.y);
        }
        
        void rotate(float x)
        {
            float sx = std::sin(x);
            float cx = std::cos(x);
            *this *= Matrix3(cx, -sx, 0.0f, sx, cx, 0.0f);
        }
        
        void scale(float x)
        {
            *this *= Matrix3(x, 0.0f, 0.0f, 0.0f, x, 0.0f);
        }
        
        void scale(Vector2 const &v)
        {
            *this *= Matrix3(v.x, 0.0f, 0.0f, 0.0f, v.y, 0.0f);
        }

        Vector2 getTranslation() const
        {
            return Vector2(c, f);
        }
        
        float getRotation() const;        
        float getScale() const;
    };

    inline Matrix3 operator*(Matrix3 const &m1, Matrix3 const &m2)
    {
        return Matrix3(m1.a * m2.a + m1.b * m2.d + m1.c * m2.g,
                       m1.a * m2.b + m1.b * m2.e + m1.c * m2.h,
                       m1.a * m2.c + m1.b * m2.f + m1.c * m2.i,

                       m1.d * m2.a + m1.e * m2.d + m1.f * m2.g,
                       m1.d * m2.b + m1.e * m2.e + m1.f * m2.h,
                       m1.d * m2.c + m1.e * m2.f + m1.f * m2.i,

                       m1.g * m2.a + m1.h * m2.d + m1.i * m2.g,
                       m1.g * m2.b + m1.h * m2.e + m1.i * m2.h,
                       m1.g * m2.c + m1.h * m2.f + m1.i * m2.i);
    }

    inline Matrix3 operator*(Matrix3 const &m, float x)
    {
        return Matrix3(m) *= x;
    }

    inline Matrix3 operator*(float x, Matrix3 const &m)
    {
        return Matrix3(m) *= x;
    }

    inline Matrix3 operator/(Matrix3 const &m, float x)
    {
        return Matrix3(m) *= 1.0f / x;
    }

    inline Vector2 transformPoint(Matrix3 const &m, Vector2 const &p)
    {
        return Vector2(m.a * p.x + m.b * p.y + m.c,
                       m.d * p.x + m.e * p.y + m.f);
    }

    inline Matrix3 &Matrix3::operator*=(Matrix3 const &m)
    {
        return *this = *this * m;
    }

    inline float Matrix3::getRotation() const
    {
        Vector2 v = (transformPoint(*this, Vector2(1.0f, 0.0f)) -
                     transformPoint(*this, Vector2(0.0f, 0.0f)));
        return std::atan2(v.y, v.x);
    }
    
    inline float Matrix3::getScale() const
    {
        Vector2 v = (transformPoint(*this, Vector2(1.0f, 0.0f)) -
                     transformPoint(*this, Vector2(0.0f, 0.0f)));
        return v.getLength();
    }
}

#endif
