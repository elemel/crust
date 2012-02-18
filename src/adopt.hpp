#ifndef CRUST_ADOPT_HPP
#define CRUST_ADOPT_HPP

#include <memory>

namespace crust {
    template <typename T>
    std::auto_ptr<T> adopt(T *ptr)
    {
        return std::auto_ptr<T>(ptr);
    }
}

#endif
