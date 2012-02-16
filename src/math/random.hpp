#ifndef CRUST_RANDOM_HPP
#define CRUST_RANDOM_HPP

namespace crust {
    // TODO: Use Boost.Random as back-end.
    class Random {
    public:
        Random();

        float getFloat();
        int getInt(int size);
    };
}

#endif
