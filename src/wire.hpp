#ifndef CRUST_WIRE_HPP
#define CRUST_WIRE_HPP

#include "error.hpp"

#include <typeinfo>

namespace crust {
    template <typename T>
    class WireConversion {
    public:
        T *ptr;

        explicit WireConversion(T *ptr) :
            ptr(ptr)
        { }

        operator T*() const
        {
            return ptr;
        }
        
        template <typename U>
        operator U *() const
        {
            U *result = dynamic_cast<U *>(ptr);
            if (result == 0) {
                throw Error("Failed to convert wired pointer");
            }
            return result;
        }
    };

    template <typename T>
    WireConversion<T> wire(T *ptr)
    {
        if (ptr == 0) {
            throw Error("Can't wire null pointer");
        }
        return WireConversion<T>(ptr);
    }
}

#endif
