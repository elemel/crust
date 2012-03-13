#include "frame_buffer.hpp"

#include "texture.hpp"

namespace crust {
    FrameBuffer::FrameBuffer() :
        handle_(0),
        colorTexture_(0)
    { }

    void FrameBuffer::create()
    {
        if (handle_ == 0) {
            glGenFramebuffers(1, &handle_);
            bind();
            GLuint colorTextureHandle = (colorTexture_ != 0) ? colorTexture_->getHandle() : 0;
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, colorTextureHandle, 0);
            unbind();
        }
    }

    void FrameBuffer::destroy()
    {
        if (handle_ != 0) {
            glDeleteFramebuffers(1, &handle_);
            handle_ = 0;
        }
    }
}
