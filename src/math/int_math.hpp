#ifndef CRUST_INT_MATH_HPP
#define CRUST_INT_MATH_HPP

namespace crust {
    class IntVector2 {
    public:
        int x, y;
        
        explicit IntVector2(int i = 0) :
            x(i),
            y(i)
        { }
        
        IntVector2(int x, int y) :
            x(x),
            y(y)
        { }
        
        IntVector2 operator-() const
        {
            return IntVector2(-x, -y);
        }
        
        IntVector2 &operator+=(IntVector2 const &v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }
        
        IntVector2 &operator-=(IntVector2 const &v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }
        
        IntVector2 &operator*=(int i)
        {
            x *= i;
            y *= i;
            return *this;
        }
        
        IntVector2 &operator/=(int i)
        {
            x /= i;
            y /= i;
            return *this;
        }
    };
    
    inline IntVector2 operator+(IntVector2 const &v1, IntVector2 const &v2)
    {
        return IntVector2(v1) += v2;
    }
    
    inline IntVector2 operator-(IntVector2 const &v1, IntVector2 const &v2)
    {
        return IntVector2(v1) -= v2;
    }
    
    inline IntVector2 operator*(IntVector2 const &v, int i)
    {
        return IntVector2(v) *= i;
    }
    
    inline IntVector2 operator*(int i, IntVector2 const &v)
    {
        return IntVector2(v) *= i;
    }    

    inline IntVector2 operator/(IntVector2 const &v, int i)
    {
        return IntVector2(v) /= i;
    }
}

#endif
