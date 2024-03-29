#ifndef CRUST_FRAME_BUFFER_HPP
#define CRUST_FRAME_BUFFER_HPP

#include <SDL/SDL_opengl.h>

namespace crust {
    class Texture;

    class FrameBuffer {
    public:
        FrameBuffer();

        ~FrameBuffer()
        {
            destroy();
        }

        Texture *getColorTexture()
        {
            return colorTexture_;
        }

        void setColorTexture(Texture *texture)
        {
            colorTexture_ = texture;
        }
        
        void create();
        void destroy();

        void invalidate()
        {
            handle_ = 0;
        }

        void bind()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, handle_);
        }
        
        void unbind()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        GLuint getHandle() const
        {
            return handle_;
        }
        
    private:
        GLuint handle_;
        Texture *colorTexture_;
        
        // Noncopyable.
        FrameBuffer(FrameBuffer const &other);
        FrameBuffer &operator=(FrameBuffer const &other);
    };
}

#endif
