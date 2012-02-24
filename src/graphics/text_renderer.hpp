#ifndef CRUST_TEXT_RENDERER_HPP
#define CRUST_TEXT_RENDERER_HPP

namespace crust {
    class Font;
    
    class TextRenderer {
    public:
        explicit TextRenderer(Font *font) :
            font_(font)
        { }

        void draw(char const *text);

        int getWidth(char const *text);
        int getHeight(char const *text);
        
    private:
        Font *font_;
    };
}

#endif
