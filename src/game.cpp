#include "game.hpp"

#include "block.hpp"
#include "config.hpp"
#include "dungeon_generator.hpp"
#include "error.hpp"
#include "font.hpp"
#include "font_reader.hpp"
#include "geometry.hpp"
#include "particle_emitter.hpp"
#include "physics_draw.hpp"
#include "text_drawer.hpp"

#include <fstream>

namespace crust {
    Game::Game(Config const *config) :
        config_(config),
        quit_(false),
        windowWidth_(config->windowWidth),
        windowHeight_(config->windowHeight),
        window_(0),
        context_(0),
        time_(0.0f),
        blockGrowthDone_(false),
        dungeonGenerationDone_(false),

        cameraX_(0.0f),
        cameraY_(0.0f),
        cameraScale_(config_->cameraScale),
    
        targetX_(0.0f),
        targetY_(0.0f),

        drawEnabled_(true),
        debugDrawEnabled_(true),
        lightingEnabled_(true),

        bounds_(Vector2(-10.0f, -10.0f), Vector2(10.0f, 10.0f)),

        fpsTime_(0.0f),
        fpsCount_(0),
    
        delauneyTriangulation_(Box2(Vector2(-20.0f, -20.0f), Vector2(20.0f, 20.0f)))
    { }
    
    Game::~Game()
    {
        if (context_) {
            SDL_GL_DeleteContext(context_);
        }
        if (window_) {
            SDL_DestroyWindow(window_);
        }
        SDL_Quit();
    }

    int Game::main(int argc, char **argv)
    {
        init();
        run();
        return 0;
    }

    float Game::getTime()
    {
        return time_;
    }

    b2World *Game::getPhysicsWorld()
    {
        return physicsWorld_.get();
    }

    void Game::BeginContact(b2Contact* contact)
    {
        b2Body *bodyA = contact->GetFixtureA()->GetBody();
        b2Body *bodyB = contact->GetFixtureB()->GetBody();
        void *userDataA = bodyA->GetUserData();
        void *userDataB = bodyB->GetUserData();
        if (userDataA == 0 && userDataB != 0) {
            addParticleToBlockCollision(bodyA, bodyB);
        }
        if (userDataA != 0 && userDataB == 0) {
            addParticleToBlockCollision(bodyB, bodyA);
        }
    }
    
    void Game::EndContact(b2Contact* contact)
    { }

    Block *Game::findBlockAtPosition(float x, float y, Block *except)
    {
        if (!bounds_.containsPoint(x, y)) {
            return endBlock_.get();
        }

        Block *block = 0;
        for (BlockIterator i = blocks_.begin(); i != blocks_.end(); ++i) {
            if (&*i != except && i->getElementAtPosition(x, y)) {
                block = &*i;
            }
        }
        return block;
    }

    float Game::getRandomFloat()
    {
        return random_.getFloat();
    }

    int Game::getRandomInt(int size)
    {
        return random_.getInt(size);
    }

    void Game::init()
    {
        initSdl();
        initWindow();
        initContext();
        time_ = 0.001f * float(SDL_GetTicks());
        initPhysics();
        particleEmitters_.push_back(new ParticleEmitter(this));
        // initBlocks();
        initFont();
        initVoronoiDiagram();
    }
    
    void Game::initSdl()
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) != 0) {
            std::stringstream message;
            message << "Failed to initialize SDL: " << SDL_GetError();
            throw Error(message.str());
        }
    }
    
    void Game::initWindow()
    {
        if (config_->fullscreen) {
            SDL_DisplayMode mode;
            if (SDL_GetCurrentDisplayMode(0, &mode) == -1) {
                std::stringstream message;
                message << "Failed to get fullscreen display mode: "
                        << SDL_GetError();
                throw Error(message.str());
            }
            windowWidth_ = mode.w;
            windowHeight_ = mode.h;
        }
        Uint32 flags = SDL_WINDOW_OPENGL;
        if (config_->fullscreen) {
            flags |= SDL_WINDOW_FULLSCREEN;
        }
        window_ = SDL_CreateWindow("Crust", SDL_WINDOWPOS_CENTERED, 
                                   SDL_WINDOWPOS_CENTERED, windowWidth_,
                                   windowHeight_, flags);
        if (window_ == 0) {
            std::stringstream message;
            message << "Failed to create window: " << SDL_GetError();
            throw Error(message.str());
        }
        if (config_->fullscreen) {
            SDL_GetWindowSize(window_, &windowWidth_, &windowHeight_);
        }
    }

    void Game::initContext()
    {
        if (config_->multisampling) {
            if (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1) == -1 ||
                SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4) == -1)
            {
                std::stringstream message;
                message << "Failed to enable multisampling: "
                        << SDL_GetError();
                throw Error(message.str());
            }
        }
        context_ = SDL_GL_CreateContext(window_);
        SDL_GL_SetSwapInterval(config_->verticalSync ? 1 : 0);
        if (config_->multisampling) {
            glEnable(GL_MULTISAMPLE);
        }
    }
    
    void Game::initPhysics()
    {
        b2Vec2 gravity(0.0f, -10.0f);
        physicsWorld_.reset(new b2World(gravity));
        physicsWorld_->SetContactListener(this);
        physicsDraw_.reset(new PhysicsDraw);
        physicsWorld_->SetDebugDraw(physicsDraw_.get());
        endBlock_.reset(new Block(this, 0.0f, 0.0f, 0.0f));
    }

    void Game::initBlocks()
    {
        for (int i = 0; i < 50; ++i) {
            float x = bounds_.p1.x + bounds_.getWidth() * getRandomFloat();
            float y = bounds_.p1.y + bounds_.getHeight() * getRandomFloat();
            float angle = -M_PI + 2.0f * M_PI * getRandomFloat();
            blocks_.push_back(new Block(this, x, y, angle));

            float brightness = 0.5f + 0.3f * getRandomFloat();
            float red = 0.3f + 0.1f * getRandomFloat();
            float green = 0.5f + 0.1f * getRandomFloat();
            float blue = 0.7f + 0.1f * getRandomFloat();
            blocks_.back().setColor(brightness * red, brightness * green, brightness * blue);

            blocks_.back().setElement(0, 0, 1);
        }
    }

    void Game::initFont()
    {
        font_.reset(new Font);
        std::ifstream in("../../../data/font.txt");
        FontReader reader;
        reader.read(&in, font_.get());
        textDrawer_.reset(new TextDrawer(font_.get()));
    }

    void Game::initVoronoiDiagram()
    {
        delauneyTriangulation_ = DelauneyTriangulation(Box2(Vector2(-20.0f, -20.0f), Vector2(20.0f, 20.0f)));
        for (int i = 0; i < 500; ++i) {
            float x = bounds_.p1.x + bounds_.getWidth() * getRandomFloat();
            float y = bounds_.p1.y + bounds_.getHeight() * getRandomFloat();
            delauneyTriangulation_.addVertex(Vector2(x, y));
        }
        voronoiDiagram_.generate(delauneyTriangulation_);
    }

    void Game::run()
    {
        while (!quit_) {
            float dt = updateTime();
            updateFps();
            handleEvents();
            handleInput();
            step(dt);
            redraw();
        }
    }
    
    float Game::updateTime()
    {
        float time = 0.001f * float(SDL_GetTicks());
        float dt = time - time_;
        time_ = time;
        return dt;
    }

    void Game::updateFps()
    {
        if (fpsTime_ < time_) {
            char buffer[64];
            sprintf(buffer, "%g FPS", float(fpsCount_));
            fpsText_ = buffer;
            fpsTime_ = time_ + 1.0f;
            fpsCount_ = 0;
        }
        ++fpsCount_;
    }
    
    void Game::handleEvents()
    {
        SDL_Event event;
#if 0
        while (SDL_PollEvent(&event)) {
            handleEvent(&event);
        }
#else
        SDL_WaitEvent(&event);
        handleEvent(&event);
#endif
    }

    void Game::handleEvent(SDL_Event *event)
    {
        switch (event->type) {
            case SDL_QUIT:
                quit_ = true;
                break;
                
            case SDL_KEYDOWN:
                handleKeyDownEvent(event);
                break;

            case SDL_MOUSEBUTTONDOWN:
                handleMouseButtonDownEvent(event);
                break;
        }
    }
    
    void Game::handleKeyDownEvent(SDL_Event *event)
    {
        switch (event->key.keysym.sym) {
            case SDLK_ESCAPE:                
                quit_ = true;
                break;

            case SDLK_BACKSPACE:
                initVoronoiDiagram();
                break;

            case SDLK_d:
                debugDrawEnabled_ = !debugDrawEnabled_;
                break;

            case SDLK_l:
                lightingEnabled_ = !lightingEnabled_;
                break;

            case SDLK_PLUS:
                cameraScale_ *= config_->cameraZoom;
                break;

            case SDLK_MINUS:
                cameraScale_ /= config_->cameraZoom;
                break;

            case SDLK_UP:
                cameraY_ += config_->cameraPan / cameraScale_;
                break;

            case SDLK_LEFT:
                cameraX_ -= config_->cameraPan / cameraScale_;
                break;

            case SDLK_RIGHT:
                cameraX_ += config_->cameraPan / cameraScale_;
                break;

            case SDLK_DOWN:
                cameraY_ -= config_->cameraPan / cameraScale_;
                break;
        }
    }

    void Game::handleMouseButtonDownEvent(SDL_Event *event)
    {
        float invScale = 2.0f / cameraScale_ / float(windowHeight_);
        float x = cameraX_ + invScale * float(event->button.x - windowWidth_ / 2);
        float y = cameraY_ + invScale * -float(event->button.y - windowHeight_ / 2);
        delauneyTriangulation_.addVertex(Vector2(x, y));
        voronoiDiagram_.generate(delauneyTriangulation_);
    }
    
    void Game::handleInput()
    {
        int x = 0;
        int y = 0;
        Uint8 buttons = SDL_GetMouseState(&x, &y);
        (void) buttons;
        float invScale = 2.0f / cameraScale_ / float(windowHeight_);
        targetX_ = cameraX_ + invScale * float(x - windowWidth_ / 2);
        targetY_ = cameraY_ + invScale * -float(y - windowHeight_ / 2);
    }

    void Game::setBlockElementAtPosition(float x, float y, int type)
    {
        BlockIterator j = blocks_.end();
        for (BlockIterator i = blocks_.begin(); i != blocks_.end(); ++i) {
            if (i->findElementNearPosition(x, y)) {
                j = i;
            }
        }
        if (j != blocks_.end()) {
            j->setElementAtPosition(x, y, type);
        }
    }
    
    void Game::step(float dt)
    {
        stepParticleEmitters(dt);
        // stepBlocks(dt);
        physicsWorld_->Step(dt, 10, 10);
        handleCollisions();
    }
    
    void Game::stepParticleEmitters(float dt)
    {
        for (ParticleEmitterIterator i = particleEmitters_.begin();
             i != particleEmitters_.end(); ++i)
        {
            i->step(dt);
        }
    }

    void Game::stepBlocks(float dt)
    {
        if (!blockGrowthDone_) {
            for (BlockIterator i = blocks_.begin(); i != blocks_.end(); ++i) {
                if (i->getNeighborCount()) {
                    blocksWithNeighbors_.push_back(&*i);
                }
            }
            if (!blocksWithNeighbors_.empty()) {
                for (int i = 0; i < 10; ++i) {
                    int j = getRandomInt(blocksWithNeighbors_.size());
                    for (int k = 0; k < 50; ++k) {
                        blocksWithNeighbors_[j]->absorbElement(1);
                    }
                }
                blocksWithNeighbors_.clear();
            } else {
                blockGrowthDone_ = true;
            }
        } else if (!dungeonGenerationDone_) {
#if 0
            DungeonGenerator generator(&random_, bounds_);
            generator.generate();
            for (int i = 0; i < generator.getRoomBoxCount(); ++i) {
                dig(generator.getRoomBox(i));
            }
            for (int i = 0; i < generator.getCorridorBoxCount(); ++i) {
                dig(generator.getCorridorBox(i));
            }
#endif
            
            for (BlockIterator i = blocks_.begin(); i != blocks_.end(); ++i) {
                i->fitPhysicsShapes();
            }

            dungeonGenerationDone_ = true;
        }
    }
    
    void Game::addParticleToBlockCollision(b2Body *particleBody,
                                           b2Body *blockBody)
    {
        particleToBlockCollisions_[particleBody] = blockBody;
    }

    void Game::handleCollisions()
    {
        for (CollisionIterator i = particleToBlockCollisions_.begin();
             i != particleToBlockCollisions_.end(); ++i)
        {
            handleParticleToBlockCollision(i->first, i->second);
        }
        particleToBlockCollisions_.clear();
    }

    void Game::handleParticleToBlockCollision(b2Body *particleBody,
                                              b2Body *blockBody)
    {
        Block *block = reinterpret_cast<Block *>(blockBody->GetUserData());
        block->absorbElement(1);
        physicsWorld_->DestroyBody(particleBody);
    }

    void Game::redraw()
    {
        clear();
        draw();
        SDL_GL_SwapWindow(window_);
    }
    
    void Game::clear()
    {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    void Game::draw()
    {
        drawWorld();
        drawOverlay();
    }
    
    void Game::drawWorld()
    {
        setWorldProjection();
        if (drawEnabled_) {
            glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            setLighting();
            drawBlocks();            
            glPopAttrib();
        }
        if (debugDrawEnabled_) {
            glPushAttrib(GL_CURRENT_BIT);
            glColor3f(0.0f, 1.0f, 0.0f);
            drawParticleEmitters();
            physicsWorld_->DrawDebugData();
            // drawBlockBounds();
            // delauneyTriangulation_.draw();
            voronoiDiagram_.draw();
            glPopAttrib();
        }
    }

    void Game::setLighting()
    {
        if (lightingEnabled_) {
            glEnable(GL_LIGHTING);
            GLfloat ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT, GL_DIFFUSE);
            glEnable(GL_NORMALIZE);

            setWorldLight();
            setCameraLight();
            setTargetLight();
        }
    }

    void Game::setWorldLight()
    { 
        glEnable(GL_LIGHT0);
        GLfloat diffuse[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
        GLfloat position[] = { 0.0f, 0.0f, 1.0f, 0.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, position);
        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0f);
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);
        glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);
    }

    void Game::setCameraLight()
    {
        glEnable(GL_LIGHT1);
        GLfloat diffuse[] = { 10.0f, 10.0f, 10.0f, 1.0f };
        GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
        GLfloat position[] = { cameraX_, cameraY_, 5.0f, 1.0f };
        glLightfv(GL_LIGHT1, GL_POSITION, position);
        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);
    }

    void Game::setTargetLight()
    {
        glEnable(GL_LIGHT2);
        GLfloat diffuse[] = { 5.0f, 5.0f, 5.0f, 1.0f };
        GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
        GLfloat position[] = { targetX_, targetY_, 2.0f, 1.0f };
        glLightfv(GL_LIGHT2, GL_POSITION, position);
        glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0f);
    }

    void Game::drawOverlay()
    {
        setOverlayProjection();
        if (config_->drawFps) {
            drawFps();
        }
    }

    void Game::drawFps()
    {
        int scale = 3;
        glPushMatrix();
        glTranslatef(float(scale), float(scale), 0.0f);
        glScalef(float(scale), float(scale), 1.0);
        textDrawer_->draw(fpsText_.c_str());
        glPopMatrix();
    }

    void Game::setWorldProjection()
    {
        float invScale = 1.0f / cameraScale_;
        float aspectRatio = float(windowWidth_) / float(windowHeight_);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(cameraX_ - invScale * aspectRatio, cameraX_ + invScale * aspectRatio,
                cameraY_ - invScale, cameraY_ + invScale, -1.0f, 1.0f);
        glMatrixMode(GL_MODELVIEW);
    }

    void Game::setOverlayProjection()
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, double(windowWidth_), 0.0, double(windowHeight_),
                -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
    }

    void Game::drawParticleEmitters()
    {
        for (ParticleEmitterIterator i = particleEmitters_.begin();
             i != particleEmitters_.end(); ++i)
        {
            i->draw();
        }
    }

    void Game::drawBlockBounds()
    {
        for (BlockIterator i = blocks_.begin(); i != blocks_.end(); ++i) {
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
    
    void Game::drawBlocks()
    {
        for (BlockIterator i = blocks_.begin(); i != blocks_.end(); ++i) {
            i->draw();
        }
    }

    void Game::dig(Box2 const &box)
    {
        for (BlockIterator i = blocks_.begin(); i != blocks_.end(); ++i) {
            i->dig(box);
        }
    }
}
