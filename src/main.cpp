#include "config.hpp"
#include "config_reader.hpp"
#include "game.hpp"

#include <fstream>

int main(int argc, char **argv)
{
    std::ifstream configFile("../../../data/config.txt");
    crust::Config config;
    crust::ConfigReader configReader(&configFile, &config);
    configReader.read();
    return crust::Game(&config).main(argc, argv);
}
