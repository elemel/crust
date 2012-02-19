#ifndef CRUST_SCENE_SERVICE_HPP
#define CRUST_SCENE_SERVICE_HPP

#include "geometry.hpp"

#include <vector>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

namespace crust {
    class Font;
    class Game;
    class Sprite;
    class Task;
    class TextRenderer;

    class SceneService {
    public:
        typedef std::vector<Sprite *> SpriteVector;
        typedef std::vector<Task *> TaskVector;

        explicit SceneService(Game *game);
        ~SceneService();

        void step(float dt);
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

        void addSprite(Sprite *sprite);
        void removeSprite(Sprite *sprite);

        void addTask(Task *task);
        void removeTask(Task *task);

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

        SpriteVector sprites_;
        TaskVector tasks_;

        GLuint targetFramebuffer_;
        GLuint targetTexture_;
        
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
        void drawSprites();
    };
}

#endif
