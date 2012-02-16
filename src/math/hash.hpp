#ifndef CRUST_HASH_HPP
#define CRUST_HASH_HPP

#include <cstddef>

namespace crust {
    // http://burtleburtle.net/bob/hash/integer.html
    inline std::size_t hashValue(std::size_t a)
    {
        a -= (a << 6);
        a ^= (a >> 17);
        a -= (a << 9);
        a ^= (a << 4);
        a -= (a << 3);
        a ^= (a << 10);
        a ^= (a >> 15);
        return a;
    }
}

#endif
