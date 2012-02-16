#ifndef CRUST_CONVERT_HPP
#define CRUST_CONVERT_HPP

#include "error.hpp"

#include <typeinfo>

namespace crust {
    template <typename T>
    class PointerConverter {
    public:
        T *ptr;

        explicit PointerConverter(T *ptr) :
            ptr(ptr)
        { }

        operator T *() const
        {
            return ptr;
        }
        
        template <typename U>
        operator U *() const
        {
            U *result = dynamic_cast<U *>(ptr);
            if (result == 0) {
                throw Error("Failed to convert pointer");
            }
            return result;
        }
    };

    template <typename T>
    PointerConverter<T> convert(T *ptr)
    {
        if (ptr == 0) {
            throw Error("Can't convert null pointer");
        }
        return PointerConverter<T>(ptr);
    }
}

#endif
