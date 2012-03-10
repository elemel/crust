#ifndef CRUST_SCOPED_HANDLE_HPP
#define CRUST_SCOPED_HANDLE_HPP

namespace crust {
    template <typename Handle>
    class ScopedHandle {
    public:
        typedef void (*Deleter)(Handle);
        
        explicit ScopedHandle(Handle handle, Deleter deleter) :
            handle_(handle),
            deleter_(deleter)
        { }
        
        ~ScopedHandle()
        {
            if (deleter_ != 0) {
                deleter_(handle_);
            }
        }
        
        Handle get() const
        {
            return handle_;
        }
        
        Handle release()
        {
            deleter_ = 0;
            return handle_;
        }
        
    private:
        Handle handle_;
        Deleter deleter_;
        
        ScopedHandle(ScopedHandle const &other);
        ScopedHandle &operator=(ScopedHandle const &other);
    };
}

#endif
