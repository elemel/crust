#ifndef CRUST_TEXTURE_HPP
#define CRUST_TEXTURE_HPP

#include <SDL/SDL_opengl.h>

namespace crust {
    class Texture {
    public:
        Texture() :
            handle_(0)
        { }
        
        ~Texture()
        {
            destroy();
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
        
        // Noncopyable.
        Texture(Texture const &other);
        Texture &operator=(Texture const &other);
    };
}

#endif
