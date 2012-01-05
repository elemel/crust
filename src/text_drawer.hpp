#ifndef CRUST_TEXT_DRAWER_HPP
#define CRUST_TEXT_DRAWER_HPP

namespace crust {
    class Font;
    
    class TextDrawer {
    public:
        explicit TextDrawer(Font *font) :
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
