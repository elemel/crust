#include "config.hpp"

namespace crust {
    Config::Config() :
        windowWidth(800),
        windowHeight(600),
        fullscreen(false),
        vsync(true),
        cameraScale(0.15f),
        cameraPan(0.1f),
        cameraZoom(1.5f),
        minCameraScale(0.02f),
        maxCameraScale(1.0f),
        drawFps(true),
        fps(60)
    { }
}
