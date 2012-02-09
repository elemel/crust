#ifndef CRUST_RENDER_MANAGER_HPP
#define CRUST_RENDER_MANAGER_HPP

#include "geometry.hpp"

#include <SDL/SDL.h>

namespace crust {
    class Font;
    class Game;
    class TextRenderer;

    class RenderManager {
    public:
        explicit RenderManager(Game *game);
        ~RenderManager();

        void draw();

        Vector2 const &getCameraPosition() const
        {
            return cameraPosition_;
        }

        void setCameraPosition(Vector2 const &position)
        {
            cameraPosition_ = position;
        }

        float getCameraScale() const
        {
            return cameraScale_;
        }

        void setCameraScale(float scale)
        {
            cameraScale_ = scale;
        }
        
        Vector2 getWorldPosition(Vector2 const &screenPosition) const;
        
    private:
        Game *game_;
        SDL_Window *window_;
        int windowWidth_;
        int windowHeight_;

        Vector2 cameraPosition_;
        float cameraScale_;
        Box2 frustum_;
        
        bool drawEnabled_;
        bool debugDrawEnabled_;
        bool lightingEnabled_;

        std::auto_ptr<Font> font_;
        std::auto_ptr<TextRenderer> textRenderer_;

        void initFont();

        void updateFrustum();
        void drawWorld();
        void setLighting();
        void setWorldLight();
        void setCameraLight();
        void setTargetLight();
        void drawOverlay();
        void drawMode();
        void drawFps();
        void setWorldProjection();
        void setOverlayProjection();
        void drawBlockBounds();
        void drawBlocks();
        void drawMonsters();
        void drawChains();
    };
}

#endif
