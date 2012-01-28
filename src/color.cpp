#include "color.hpp"

namespace crust {
    namespace {
        unsigned char parseHexDigit(char digit)
        {
            if ('0' <= digit && digit <= '9') {
                return digit - '0';
            } else if ('a' <= digit && digit <= 'f') {
                return 10 + digit - 'a';
            } else if ('A' <= digit && digit <= 'F') {
                return 10 + digit - 'A';
            } else {
                return 0;
            }
        }        
    }
    
    Color3 parseColor3(char const *str)
    {
        Color3 color;
        
        if (str[0] == '#') {
            ++str;
        }
        
        switch (std::strlen(str)) {
            case 3:
                color.red = 17 * parseHexDigit(str[0]);
                color.green = 17 * parseHexDigit(str[1]);
                color.blue = 17 * parseHexDigit(str[2]);
                break;
                
            case 6:
                color.red = 16 * parseHexDigit(str[0]) + parseHexDigit(str[1]);
                color.green = 16 * parseHexDigit(str[2]) + parseHexDigit(str[3]);
                color.blue = 16 * parseHexDigit(str[4]) + parseHexDigit(str[5]);
                break;
        }
        
        return color;
    }
    
    Color4 parseColor4(char const *str)
    {
        Color4 color;
        
        if (str[0] == '#') {
            ++str;
        }
        
        switch (std::strlen(str)) {
            case 4:
                color.alpha = 17 * parseHexDigit(str[3]);
            case 3:
                color.red = 17 * parseHexDigit(str[0]);
                color.green = 17 * parseHexDigit(str[1]);
                color.blue = 17 * parseHexDigit(str[2]);
                break;
                
            case 8:
                color.alpha = 16 * parseHexDigit(str[6]) + parseHexDigit(str[7]);
            case 6:
                color.red = 16 * parseHexDigit(str[0]) + parseHexDigit(str[1]);
                color.green = 16 * parseHexDigit(str[2]) + parseHexDigit(str[3]);
                color.blue = 16 * parseHexDigit(str[4]) + parseHexDigit(str[5]);
                break;
        }
        
        return color;
    }
}
