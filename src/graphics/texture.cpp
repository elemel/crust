#include "texture.hpp"

namespace crust {
    void Texture::create()
    {
        if (handle_ == 0) {
            glGenTextures(1, &handle_);
        }
    }
    
    void Texture::destroy()
    {
        if (handle_ != 0) {
            glDeleteTextures(1, &handle_);
            handle_ = 0;
        }
    }
}
