#ifndef CRUST_CONFIG_READER_HPP
#define CRUST_CONFIG_READER_HPP

#include <iostream>

namespace crust {
    class Config;

    class ConfigReader {
    public:
        ConfigReader(std::istream *source, Config *target);

        void read();

    private:
        std::istream *source_;
        Config *target_;
        std::string line_;
        std::string key_;
        std::string value_;

        void readValue();

        bool parseBool(char const *arg);
        int parseInt(char const *arg);
        float parseFloat(char const *arg);
    };
}

#endif
