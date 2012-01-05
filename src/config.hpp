#ifndef CRUST_CONFIG_HPP
#define CRUST_CONFIG_HPP

namespace crust {
    class Config {
    public:
        int windowWidth;
        int windowHeight;
        bool fullscreen;
        bool multisampling;
        bool verticalSync;
        float cameraScale;
        float cameraPan;
        float cameraZoom;
        bool drawFps;

        Config();
    };
}

#endif
