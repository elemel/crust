#include "font_reader.hpp"

#include "error.hpp"
#include "font.hpp"

#include <sstream>
#include <vector>

namespace crust {
    void FontReader::read(std::istream *source, Font *target)
    {
        enum {
            keyState,
            glyphState
        } state = keyState;
        std::string line;
        while (std::getline(*source, line)) {
            strip(&line);
            switch (state) {
                case keyState:
                    if (!line.empty()) {
                        keyLine_ = line;
                        state = glyphState;
                        glyphWidth_ = 0;
                        glyphHeight_ = 0;
                    }
                    break;

                case glyphState:
                    if (line.empty()) {
                        if (glyphHeight_) {
                            addGlyph(target);
                            state = keyState;
                        }
                    } else {
                        if (glyphWidth_ == 0) {
                            glyphWidth_ = line.size();
                        } else {
                            if (line.size() != glyphWidth_) {
                                std::stringstream message;
                                message << "Glyph width mismatch: " << line;
                                throw Error(message.str());
                            }
                        }
                        if (glyphHeight_ == glyphLines_.size()) {
                            glyphLines_.resize(glyphLines_.size() + 1);
                        }
                        glyphLines_[glyphHeight_] = line;
                        ++glyphHeight_;
                    }
                    break;
            }
        }
    }

    void FontReader::strip(std::string *line)
    {
        while (!line->empty() && std::isspace((*line)[line->size() - 1])) {
            line->resize(line->size() - 1);
        }
    }

    void FontReader::addGlyph(Font *target)
    {
        char key = getKey();
        target->addGlyph(key, glyphWidth_, glyphHeight_);
        for (int y = 0; y < glyphHeight_; ++y) {
            for (int x = 0; x < glyphWidth_; ++x) {
                bool value = (glyphLines_[glyphHeight_ - y - 1][x] == '#');
                target->setGlyphPixel(key, x, y, value);
            }
        }
    }

    char FontReader::getKey()
    {
        char key = 0;
        if (keyLine_ == "DOT") {
            key = '.';
        } else if (keyLine_ == "HASH") {
            key = '#';
        } else if (keyLine_ == "SPACE") {
            key = ' ';
        } else {
            key = keyLine_[0];
        }
        return key;
    }
}
