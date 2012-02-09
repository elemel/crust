#include "render_manager.hpp"

#include "block.hpp"
#include "chain.hpp"
#include "config.hpp"
#include "font.hpp"
#include "font_reader.hpp"
#include "game.hpp"
#include "monster.hpp"
#include "text_renderer.hpp"

#include <fstream>

namespace crust {
    RenderManager::RenderManager(Game *game) :
        game_(game),
        window_(game->getWindow()),
        windowWidth_(0),
        windowHeight_(0),

        cameraScale_(game->getConfig()->cameraScale),
    
        drawEnabled_(true),
        debugDrawEnabled_(false),
        lightingEnabled_(true)
    {
        SDL_GetWindowSize(window_, &windowWidth_, &windowHeight_);

        initFont();
    }

    RenderManager::~RenderManager()
    { }
    
    
    void RenderManager::draw()
    {
        updateFrustum();
        drawWorld();
        drawOverlay();
    }
    
    Vector2 RenderManager::getWorldPosition(Vector2 const &screenPosition) const
    {
        float invScale = 2.0f / cameraScale_ / float(windowHeight_);
        Vector2 worldPosition;
        worldPosition.x = cameraPosition_.x + invScale * (screenPosition.x - float(windowWidth_ / 2));
        worldPosition.y = cameraPosition_.y + invScale * -(screenPosition.y - float(windowHeight_ / 2));
        return worldPosition;
    }

    void RenderManager::initFont()
    {
        font_.reset(new Font);
        std::ifstream in("../../../data/font.txt");
        FontReader reader;
        reader.read(&in, font_.get());
        textRenderer_.reset(new TextRenderer(font_.get()));
    }
    
    void RenderManager::updateFrustum()
    {
        float invScale = 1.0f / cameraScale_;
        float aspectRatio = float(windowWidth_) / float(windowHeight_);
        frustum_ = Box2(Vector2(cameraPosition_.x - invScale * aspectRatio,
                                cameraPosition_.y - invScale),
                        Vector2(cameraPosition_.x + invScale * aspectRatio,
                                cameraPosition_.y + invScale));
    }

    void RenderManager::drawWorld()
    {
        setWorldProjection();
        if (drawEnabled_) {
            glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
            // glEnable(GL_BLEND);
            // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            setLighting();
            drawBlocks();
            drawMonsters();
            drawChains();
            glPopAttrib();
        }
        if (debugDrawEnabled_) {
            glPushAttrib(GL_CURRENT_BIT);
            glColor3f(0.0f, 1.0f, 0.0f);
            game_->getPhysicsWorld()->DrawDebugData();
            // drawBlockBounds();
            // glColor3f(0.0f, 0.5f, 1.0f);
            // delauneyTriangulation_.draw();
            // glColor3f(1.0f, 0.5f, 0.0f);
            // voronoiDiagram_.draw();
            glPopAttrib();
        }
    }
    
    void RenderManager::setLighting()
    {
        if (lightingEnabled_) {
            glEnable(GL_LIGHTING);
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
    
    void RenderManager::setWorldLight()
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
    
    void RenderManager::setCameraLight()
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
    
    void RenderManager::setTargetLight()
    {
        Game::MonsterVector const &monsters = game_->getMonsters();
        if (!monsters.empty()) {
            glEnable(GL_LIGHT2);
            GLfloat diffuse[] = { 2.0f, 2.0f, 2.0f, 1.0f };
            GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
            glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
            Vector2 const &targetPosition = monsters.front().getTargetPosition();
            GLfloat position[] = { targetPosition.x, targetPosition.y, 1.0f, 1.0f };
            glLightfv(GL_LIGHT2, GL_POSITION, position);
            glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0f);
            glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 1.0f);
            glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0f);
        }
    }
    
    void RenderManager::drawOverlay()
    {
        setOverlayProjection();
        drawMode();
        if (game_->getConfig()->drawFps) {
            drawFps();
        }
    }
    
    void RenderManager::drawMode()
    {
        int scale = 3;
        glPushMatrix();
        glTranslatef(0.0f, float(windowHeight_) - float(scale) * textRenderer_->getHeight("X"), 0.0f);
        glTranslatef(float(scale), -float(scale), 0.0f);
        glScalef(float(scale), float(scale), 1.0);
        switch (game_->getMode()) {
            case Game::DIG_MODE:
                textRenderer_->draw("DIG");
                break;

            case Game::CHAIN_MODE:
                textRenderer_->draw("CHAIN");
                break;

            case Game::LIFT_MODE:
                textRenderer_->draw("LIFT");
                break;

            case Game::COLLAPSE_MODE:
                textRenderer_->draw("COLLAPSE");
                break;

            default:
                break;
        }
        glPopMatrix();
    }
    
    void RenderManager::drawFps()
    {
        int scale = 3;
        glPushMatrix();
        glTranslatef(float(scale), float(scale), 0.0f);
        glScalef(float(scale), float(scale), 1.0);
        textRenderer_->draw(game_->getFpsText());
        glPopMatrix();
    }
    
    void RenderManager::setWorldProjection()
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(frustum_.p1.x, frustum_.p2.x, frustum_.p1.y, frustum_.p2.y,
                -1.0f, 1.0f);
        glMatrixMode(GL_MODELVIEW);
    }
    
    void RenderManager::setOverlayProjection()
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, double(windowWidth_), 0.0, double(windowHeight_),
                -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
    }
    
    void RenderManager::drawBlockBounds()
    {
        Game::BlockVector const &blocks = game_->getBlocks();
        for (Game::ConstBlockIterator i = blocks.begin(); i != blocks.end(); ++i) {
            Box2 bounds = i->getBounds();
            if (!bounds.isEmpty()) {
                float x = bounds.p1.x;
                float y = bounds.p1.y;
                float width = bounds.getWidth();
                float height = bounds.getHeight();
                
                glBegin(GL_LINE_LOOP);
                glVertex2f(x, y);
                glVertex2f(x + width, y);
                glVertex2f(x + width, y + height);
                glVertex2f(x, y + height);
                glEnd();
            }
        }
    }
    
    void RenderManager::drawBlocks()
    {
        Game::BlockVector const &blocks = game_->getBlocks();
        for (Game::ConstBlockIterator i = blocks.begin(); i != blocks.end(); ++i) {
            if (intersects(frustum_, i->getBounds())) {
                i->draw();
            }
        }
    }

    void RenderManager::drawMonsters()
    {
        Game::MonsterVector const &monsters = game_->getMonsters();
        for (Game::ConstMonsterIterator i = monsters.begin(); i != monsters.end(); ++i) {
            i->draw();
        }
    }

    void RenderManager::drawChains()
    {
        Game::ChainVector const &chains = game_->getChains();
        for (Game::ConstChainIterator i = chains.begin(); i != chains.end(); ++i) {
            i->draw();
        }
    }
}
