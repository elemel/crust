#include "frame_buffer.hpp"

namespace crust {
    void FrameBuffer::create()
    {
        if (handle_ == 0) {
            glGenFramebuffers(1, &handle_);
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
