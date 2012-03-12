#ifndef CRUST_TEXTURE_HPP
#define CRUST_TEXTURE_HPP

#include <vector>
#include <SDL/SDL_opengl.h>

namespace crust {
    class Texture {
    public:
        Texture();
        ~Texture();

        GLint getInternalFormat() const
        {
            return internalFormat_;
        }
        
        void setInternalFormat(GLint format)
        {
            internalFormat_ = format;
        }

        GLsizei getWidth() const
        {
            return width_;
        }

        GLsizei geHeight() const
        {
            return height_;
        }

        void setSize(GLsizei width, GLsizei height)
        {
            width_ = width;
            height_ = height;
        }

        GLenum getFormat() const
        {
            return format_;
        }

        void setFormat(GLenum format)
        {
            format_ = format;
        }

        GLenum getType() const
        {
            return type_;
        }

        void setType(GLenum type)
        {
            type_ = type;
        }
        
        GLvoid const *getPixels() const
        {
            return pixels_.empty() ? 0 : &pixels_.front();
        }

        GLsizei getPixelSize() const
        {
            return GLsizei(pixels_.size());
        }
        
        void setPixels(GLvoid const *pixels, GLsizei size);

        GLint getMinFilter() const
        {
            return minFilter_;
        }

        GLint getMagFilter() const
        {
            return magFilter_;
        }

        void setFilters(GLint minFilter, GLint magFilter)
        {
            minFilter_ = minFilter;
            magFilter_ = magFilter;
        }
        
        void create();
        void destroy();
        
        void invalidate()
        {
            handle_ = 0;
        }
        
        void bind()
        {
            glBindTexture(GL_TEXTURE_2D, handle_);
        }
        
        void unbind()
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        GLuint getHandle() const
        {
            return handle_;
        }
        
    private:
        GLuint handle_;

        GLint internalFormat_;
        GLsizei width_;
        GLsizei height_;
        GLenum format_;
        GLenum type_;
        std::vector<unsigned char> pixels_;

        GLint minFilter_;
        GLint magFilter_;
        
        // Noncopyable.
        Texture(Texture const &other);
        Texture &operator=(Texture const &other);
    };
}

#endif

