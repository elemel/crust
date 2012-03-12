#include "texture.hpp"

namespace crust {
    Texture::Texture() :
        handle_(0),
    
        internalFormat_(GL_RGBA),
        width_(0),
        height_(0),
        format_(GL_RGBA),
        type_(GL_UNSIGNED_BYTE),

        minFilter_(GL_LINEAR),
        magFilter_(GL_LINEAR)
    { }
    
    Texture::~Texture()
    {
        destroy();
    }

    void Texture::setPixels(GLvoid const *pixels, GLsizei size)
    {
        unsigned char const *pixelBytes = reinterpret_cast<unsigned char const *>(pixels);
        pixels_.assign(pixelBytes, pixelBytes + size);
    }
    
    void Texture::create()
    {
        if (handle_ == 0) {
            glGenTextures(1, &handle_);
            bind();
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat_, width_, height_, 0, format_, type_, getPixels());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter_);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter_);
            unbind();
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
