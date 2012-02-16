#ifndef CRUST_FONT_READER_HPP
#define CRUST_FONT_READER_HPP

#include <iostream>
#include <string>
#include <vector>

namespace crust {
    class Font;

    class FontReader {
    public:
        FontReader() :
            glyphWidth_(0),
            glyphHeight_(0)
        { }

        void read(std::istream *source, Font *target);

    private:
        std::string keyLine_;
        std::vector<std::string> glyphLines_;
        int glyphWidth_;
        int glyphHeight_;

        void strip(std::string *line);
        void addGlyph(Font *target);
        char getKey();
    };
}

#endif
