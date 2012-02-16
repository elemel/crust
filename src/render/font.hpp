#ifndef CRUST_FONT_HPP
#define CRUST_FONT_HPP

#include "error.hpp"

#include <boost/dynamic_bitset.hpp>

namespace crust {
    class Font {
    public:
        typedef unsigned char Byte;

        Font();
        
        void addGlyph(char key, int width, int height);
        
        int getGlyphHeight(char key) const
        {
            return height_;
        }
        
        int getGlyphWidth(char key)
        {
            return glyphWidths_[Byte(key)];
        }
        
        bool getGlyphPixel(char key, int x, int y);
        void setGlyphPixel(char key, int x, int y, bool value);
        
    private:
        Byte height_;
        int glyphOffsets_[256];
        Byte glyphWidths_[256];
        boost::dynamic_bitset<> glyphData_;
    };
}

#endif
