#include "text_renderer.hpp"

#include "font.hpp"

#include <SDL/SDL_opengl.h>

namespace crust {
    void TextRenderer::draw(char const *text)
    {
        int x = 0;
        int y = 0;
        glBegin(GL_QUADS);
        for (char const *glyph = text; *glyph; ++glyph) {
            if (glyph != text) {
                x += 1;
            }
            int width = font_->getGlyphWidth(*glyph);
            int height = font_->getGlyphHeight(*glyph);
            for (int dy = 0; dy < height; ++dy) {
                for (int dx = 0; dx < width; ++dx) {
                    if (font_->getGlyphPixel(*glyph, dx, dy)) {
                        glVertex2i(x + dx, y + dy);
                        glVertex2i(x + dx + 1, y + dy);
                        glVertex2i(x + dx + 1, y + dy + 1);
                        glVertex2i(x + dx, y + dy + 1);
                    }
                }
            }
            x += width;
        }
        glEnd();
    }

    int TextRenderer::getWidth(char const *text)
    {
        int width = 0;
        for (char const *glyph = text; *glyph; ++glyph) {
            if (glyph != text) {
                width += 1;
            }
            width += font_->getGlyphWidth(*glyph);
        }
        return width;
    }

    int TextRenderer::getHeight(char const *text)
    {
        int height = 0;
        for (char const *glyph = text; *glyph; ++glyph) {
            height = std::max(height, font_->getGlyphHeight(*glyph));
        }
        return height;
    }
}
