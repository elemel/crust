#ifndef CRUST_TEXT_RENDERER_HPP
#define CRUST_TEXT_RENDERER_HPP

namespace crust {
    class Font;
    class Sprite;
    
    class TextRenderer {
    public:
        explicit TextRenderer(Font *font) :
            font_(font)
        { }

        void draw(char const *text);
        void draw(char const *text, Sprite *target);

        int getWidth(char const *text);
        int getHeight(char const *text);
        
    private:
        Font *font_;
    };
}

#endif
