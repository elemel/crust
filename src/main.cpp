#include "config.hpp"
#include "config_reader.hpp"
#include "error.hpp"
#include "game.hpp"

#include <fstream>

int main(int argc, char **argv)
{
    std::ifstream configFile("../../../data/config.txt");
    crust::Config config;
    crust::ConfigReader configReader(&configFile, &config);
    configReader.read();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) != 0) {
        std::stringstream message;
        message << "Failed to initialize SDL: " << SDL_GetError();
        throw crust::Error(message.str());
    }

    crust::Game(&config).run();
    return 0;
}
