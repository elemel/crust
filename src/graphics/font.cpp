#include "font.hpp"

#include <algorithm>
#include <sstream>

namespace crust {
    Font::Font() :
        height_(0)
    {
        std::fill(glyphOffsets_, glyphOffsets_ + 256, 0);
        std::fill(glyphWidths_, glyphWidths_ + 256, 0);
    }
    
    void Font::addGlyph(char key, int width, int height)
    {
        if (height_ && height != height_) {
            std::stringstream message;
            message << "Glyph height mismatch: " << key;
            throw Error(message.str());
        }
        
        height_ = height;
        glyphOffsets_[Byte(key)] = glyphData_.size();
        glyphWidths_[Byte(key)] = width;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                glyphData_.push_back(false);
            }
        }
    }

    bool Font::getGlyphPixel(char key, int x, int y)
    {
        int offset = glyphOffsets_[Byte(key)];
        int width = glyphWidths_[Byte(key)];
        int index = offset + y * width + x;
        return glyphData_[index];
    }
    
    void Font::setGlyphPixel(char key, int x, int y, bool value)
    {
        int offset = glyphOffsets_[Byte(key)];
        int width = glyphWidths_[Byte(key)];
        int index = offset + y * width + x;
        glyphData_[index] = value;
    }
}
