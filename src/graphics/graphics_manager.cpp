#include "graphics_manager.hpp"

#include "actor.hpp"
#include "block_physics_component.hpp"
#include "config.hpp"
#include "convert.hpp"
#include "font.hpp"
#include "font_reader.hpp"
#include "game.hpp"
#include "monster_control_component.hpp"
#include "physics_manager.hpp"
#include "sprite.hpp"
#include "task.hpp"
#include "text_renderer.hpp"

#include <algorithm>
#include <fstream>

namespace crust {
    GraphicsManager::GraphicsManager(Game *game) :
        game_(game),
        window_(game->getWindow()),
        windowWidth_(0),
        windowHeight_(0),

        cameraScale_(game->getConfig()->cameraScale),
    
        drawEnabled_(true),
        debugDrawEnabled_(false),
        lightingEnabled_(true),

        shaderProgram_(0)
    {
        SDL_GetWindowSize(window_, &windowWidth_, &windowHeight_);

        initFont();
        initShaders();
    }

    GraphicsManager::~GraphicsManager()
    {
        if (shaderProgram_) {
            glDeleteProgram(shaderProgram_);
        }
        if (vertexShader_) {
            glDeleteShader(vertexShader_);
        }
        if (fragmentShader_) {
            glDeleteShader(fragmentShader_);
        }
    }

    void GraphicsManager::step(float dt)
    {
        for (TaskVector::iterator i = tasks_.begin(); i != tasks_.end(); ++i) {
            (*i)->step(dt);
        }
    }
    
    void GraphicsManager::draw()
    {
        updateFrustum();

        drawWorld();

        drawOverlay();
    }
    
    Vector2 GraphicsManager::getWorldPosition(Vector2 const &screenPosition) const
    {
        float invScale = 2.0f / cameraScale_ / float(windowHeight_);
        Vector2 worldPosition;
        worldPosition.x = cameraPosition_.x + invScale * (screenPosition.x - float(windowWidth_ / 2));
        worldPosition.y = cameraPosition_.y + invScale * -(screenPosition.y - float(windowHeight_ / 2));
        return worldPosition;
    }

    void GraphicsManager::addSprite(Sprite *sprite)
    {
        sprites_.push_back(sprite);
    }

    void GraphicsManager::removeSprite(Sprite *sprite)
    {
        SpriteVector::iterator i = std::find(sprites_.begin(), sprites_.end(), sprite);
        sprites_.erase(i);
    }

    void GraphicsManager::addTask(Task *task)
    {
        tasks_.push_back(task);
    }
    
    void GraphicsManager::removeTask(Task *task)
    {
        TaskVector::iterator i = std::find(tasks_.begin(), tasks_.end(), task);
        tasks_.erase(i);
    }

    void GraphicsManager::initFont()
    {
        font_.reset(new Font);
        std::ifstream in("../../../data/font.txt");
        FontReader reader;
        reader.read(&in, font_.get());
        textRenderer_.reset(new TextRenderer(font_.get()));
    }

    void GraphicsManager::initShaders()
    {
        vertexShader_ = shaderFactory_.compileShader(GL_VERTEX_SHADER, "../../../data/vertex.glsl");
        fragmentShader_ = shaderFactory_.compileShader(GL_FRAGMENT_SHADER, "../../../data/fragment.glsl");
        shaderProgram_ = shaderFactory_.linkProgram(vertexShader_, fragmentShader_);
    }

    void GraphicsManager::updateFrustum()
    {
        float invScale = 1.0f / cameraScale_;
        float aspectRatio = float(windowWidth_) / float(windowHeight_);
        frustum_ = Box2(Vector2(cameraPosition_.x - invScale * aspectRatio,
                                cameraPosition_.y - invScale),
                        Vector2(cameraPosition_.x + invScale * aspectRatio,
                                cameraPosition_.y + invScale));
    }

    void GraphicsManager::drawWorld()
    {
        setWorldProjection();
        if (drawEnabled_) {
            // glEnable(GL_LIGHTING);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            // setLighting();
            glUseProgram(shaderProgram_);
            drawSprites();
            glUseProgram(0);
            glDisable(GL_BLEND);
            // glDisable(GL_LIGHTING);
        }
        if (debugDrawEnabled_) {
            glColor3f(0.0f, 1.0f, 0.0f);
            game_->getPhysicsManager()->getWorld()->DrawDebugData();
            // glColor3f(0.0f, 0.5f, 1.0f);
            // delauneyTriangulation_.draw();
            // glColor3f(1.0f, 0.5f, 0.0f);
            // voronoiDiagram_.draw();
        }
    }
    
    void GraphicsManager::setLighting()
    {
        if (lightingEnabled_) {
            GLfloat ambient[] = { 0.05f, 0.05f, 0.05f, 1.0f };
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT, GL_DIFFUSE);
            glEnable(GL_NORMALIZE);
            
            setWorldLight();
            setCameraLight();
            setTargetLight();
        }
    }
    
    void GraphicsManager::setWorldLight()
    { 
        glEnable(GL_LIGHT0);
        GLfloat diffuse[] = { 0.1f, 0.1f, 0.1f, 1.0f };
        GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
        GLfloat position[] = { 0.0f, 0.0f, 1.0f, 0.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, position);
        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0f);
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);
        glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);
    }
    
    void GraphicsManager::setCameraLight()
    {
        glEnable(GL_LIGHT1);
        GLfloat diffuse[] = { 4.0f, 4.0f, 4.0f, 1.0f };
        GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
        GLfloat position[] = { cameraPosition_.x, cameraPosition_.y, 5.0f, 1.0f };
        glLightfv(GL_LIGHT1, GL_POSITION, position);
        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);
    }
    
    void GraphicsManager::setTargetLight()
    {
        if (game_->getPlayerActor()) {
            MonsterControlComponent *controlComponent = convert(game_->getPlayerActor()->getControlComponent());

            glEnable(GL_LIGHT2);
            GLfloat diffuse[] = { 2.0f, 2.0f, 2.0f, 1.0f };
            GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
            glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
            Vector2 const &targetPosition = controlComponent->getTargetPosition();
            GLfloat position[] = { targetPosition.x, targetPosition.y, 1.0f, 1.0f };
            glLightfv(GL_LIGHT2, GL_POSITION, position);
            glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0f);
            glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 1.0f);
            glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0f);
        }
    }
    
    void GraphicsManager::drawOverlay()
    {
        setOverlayProjection();
        drawMode();
        if (game_->getConfig()->drawFps) {
            drawFps();
        }
    }
    
    void GraphicsManager::drawMode()
    {
        if (game_->getPlayerActor()) {
            MonsterControlComponent *controlComponent = convert(game_->getPlayerActor()->getControlComponent());
            
            int scale = 3;
            glPushMatrix();
            glTranslatef(0.0f, float(windowHeight_) - float(scale) * textRenderer_->getHeight("X"), 0.0f);
            glTranslatef(2.0f * float(scale), -2.0f * float(scale), 0.0f);
            glScalef(float(scale), float(scale), 1.0);
            switch (controlComponent->getActionMode()) {
                case MonsterControlComponent::MINE_MODE:
                    textRenderer_->draw("MINE");
                    break;
                    
                case MonsterControlComponent::DRAG_MODE:
                    textRenderer_->draw("DRAG");
                    break;
                    
                case MonsterControlComponent::DROP_MODE:
                    textRenderer_->draw("DROP");
                    break;
                    
                default:
                    break;
            }
            glPopMatrix();
        }
    }
    
    void GraphicsManager::drawFps()
    {
        int scale = 3;
        glPushMatrix();
        glTranslatef(2.0f * float(scale), 2.0f * float(scale), 0.0f);
        glScalef(float(scale), float(scale), 1.0);
        textRenderer_->draw(game_->getFpsText());
        glPopMatrix();
    }
    
    void GraphicsManager::setWorldProjection()
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(frustum_.p1.x, frustum_.p2.x, frustum_.p1.y, frustum_.p2.y,
                -1.0f, 1.0f);
        glMatrixMode(GL_MODELVIEW);
    }
    
    void GraphicsManager::setOverlayProjection()
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, double(windowWidth_), 0.0, double(windowHeight_),
                -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
    }

    void GraphicsManager::drawSprites()
    {
        GLint colorTexturelocation = glGetUniformLocation(shaderProgram_, "colorTexture");
        glUniform1i(colorTexturelocation, 0);
        GLint normalAndShadowTexturelocation = glGetUniformLocation(shaderProgram_, "normalAndShadowTexture");
        glUniform1i(normalAndShadowTexturelocation, 1);
        GLint offsetLocation = glGetUniformLocation(shaderProgram_, "offset");
        for (SpriteVector::iterator i = sprites_.begin(); i != sprites_.end(); ++i) {
            float scale = 0.25 * 10.0f * frustum_.getHeight() / float(windowHeight_);
            IntVector2 size = (*i)->getSize();
            Vector2 offset(scale / float(size.x), scale / float(size.y));
            glUniform2f(offsetLocation, GLfloat(offset.x), GLfloat(offset.y));
            (*i)->draw();
        }
    }
}
