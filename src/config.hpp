#ifndef CRUST_CONFIG_HPP
#define CRUST_CONFIG_HPP

namespace crust {
    class Config {
    public:
        int windowWidth;
        int windowHeight;
        bool fullscreen;
        bool multisampling;
        bool supersampling;
        bool vsync;
        float cameraScale;
        float cameraPan;
        float cameraZoom;
        float minCameraScale;
        float maxCameraScale;
        bool drawFps;
        int fps;

        Config();
    };
}

#endif
