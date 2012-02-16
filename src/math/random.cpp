#include "random.hpp"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <limits>

namespace crust {
    Random::Random()
    {
        std::srand(std::time(0));
    }

    float Random::getFloat()
    {
        return std::min(float(std::rand()) / float(RAND_MAX),
                        1.0f - std::numeric_limits<float>::epsilon());
    }

    int Random::getInt(int size)
    {
        return std::rand() % size;
    }
}
