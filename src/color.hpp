#ifndef CRUST_COLOR_HPP
#define CRUST_COLOR_HPP

#include <cstring>

namespace crust {
    class Color3 {
    public:
        unsigned char red;
        unsigned char green;
        unsigned char blue;

        explicit Color3(unsigned char value = 0) :
            red(value),
            green(value),
            blue(value)
        { }

        Color3(unsigned char red, unsigned char green, unsigned char blue) :
            red(red),
            green(green),
            blue(blue)
        { }
    };

    inline bool operator==(Color3 const &c1, Color3 const &c2)
    {
        return c1.red == c2.red && c1.green == c2.green && c1.blue == c2.blue;
    }

    inline bool operator!=(Color3 const &c1, Color3 const &c2)
    {
        return !(c1 == c2);
    }

    class Color4 {
    public:
        unsigned char red;
        unsigned char green;
        unsigned char blue;
        unsigned char alpha;

        explicit Color4(unsigned char value = 0, unsigned char alpha = 255) :
            red(value),
            green(value),
            blue(value),
            alpha(alpha)
        { }

        Color4(unsigned char red, unsigned char green, unsigned char blue,
               unsigned char alpha = 255) :
            red(red),
            green(green),
            blue(blue),
            alpha(alpha)
        { }
    };

    inline bool operator==(Color4 const &c1, Color4 const &c2)
    {
        return (c1.red == c2.red && c1.green == c2.green &&
                c1.blue == c2.blue && c1.alpha == c2.alpha);
    }
    
    inline bool operator!=(Color4 const &c1, Color4 const &c2)
    {
        return !(c1 == c2);
    }

    class FloatColor3 {
    public:
        float red;
        float green;
        float blue;
        
        explicit FloatColor3(float value = 0.0f) :
            red(value),
            green(value),
            blue(value)
        { }

        FloatColor3(float red, float green, float blue) :
            red(red),
            green(green),
            blue(blue)
        { }
    };
    
    class FloatColor4 {
    public:
        float red;
        float green;
        float blue;
        float alpha;
        
        explicit FloatColor4(float value = 0.0f, float alpha = 1.0f) :
            red(value),
            green(value),
            blue(value),
            alpha(alpha)
        { }
        
        FloatColor4(float red, float green, float blue, float alpha = 1.0f) :
            red(red),
            green(green),
            blue(blue),
            alpha(alpha)
        { }
    };

    Color3 parseColor3(char const *str);
    Color4 parseColor4(char const *str);
}

#endif
