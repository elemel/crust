#include "config.hpp"

namespace crust {
    Config::Config() :
        windowWidth(800),
        windowHeight(600),
        fullscreen(false),
        multisampling(true),
        verticalSync(false),
        cameraScale(0.08f),
        cameraPan(0.1f),
        cameraZoom(1.5f),
        drawFps(true)
    { }
}
