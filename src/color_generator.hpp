#ifndef CRUST_COLOR_GENERATOR_HPP
#define CRUST_COLOR_GENERATOR_HPP

namespace crust {
    class Color3;
    class Random;
    
    class ColorGenerator {
    public:
        explicit ColorGenerator(Random *random);
        
        Color3 generateColor();

    private:
        Random *random_;
    };
}

#endif
