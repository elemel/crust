#include "game.hpp"

#include "block.hpp"
#include "chain.hpp"
#include "config.hpp"
#include "dungeon_generator.hpp"
#include "error.hpp"
#include "font.hpp"
#include "font_reader.hpp"
#include "geometry.hpp"
#include "monster.hpp"
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

        appTime_(0.0),
        time_(0.0),

        cameraScale_(config_->cameraScale),

        drawEnabled_(true),
        debugDrawEnabled_(false),
        lightingEnabled_(true),

        bounds_(Vector2(-15.0f, -15.0f), Vector2(15.0f, 15.0f)),

        fpsTime_(0.0),
        fpsCount_(0),
    
        delauneyTriangulation_(bounds_),
        dungeonGenerator_(&random_, bounds_)
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

    void Game::BeginContact(b2Contact* contact)
    {
        // b2Body *bodyA = contact->GetFixtureA()->GetBody();
        // b2Body *bodyB = contact->GetFixtureB()->GetBody();
        // void *userDataA = bodyA->GetUserData();
        // void *userDataB = bodyB->GetUserData();
    }
    
    void Game::EndContact(b2Contact* contact)
    { }

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
        initPhysics();
        initFont();
        initVoronoiDiagram();
        initBlocks();
        initDungeon();
        initMonsters();
        initChains();
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
        SDL_GL_SetSwapInterval(config_->vsync ? 1 : 0);
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
        Box2 vertexBounds = bounds_;
        vertexBounds.pad(5.0f);
        Box2 triangulationBounds = vertexBounds;
        triangulationBounds.pad(5.0f);
        delauneyTriangulation_ = DelauneyTriangulation(triangulationBounds);
        for (int i = 0; i < 2000; ++i) {
            float x = vertexBounds.p1.x + vertexBounds.getWidth() * getRandomFloat();
            float y = vertexBounds.p1.y + vertexBounds.getHeight() * getRandomFloat();
            delauneyTriangulation_.addVertex(Vector2(x, y));
        }
        voronoiDiagram_.generate(delauneyTriangulation_);
    }

    void Game::initBlocks()
    {
        blocks_.clear();
        Box2 paddedBounds = bounds_;
        paddedBounds.pad(2.0f);
        for (int i = 0; i < voronoiDiagram_.getPolygonCount(); ++i) {
            Polygon2 polygon = voronoiDiagram_.getPolygon(i);
            if (contains(paddedBounds, polygon)) {
                blocks_.push_back(new Block(this, polygon));
                
                float brightness = 0.5f + 0.3f * getRandomFloat();
                float red = 0.3f + 0.1f * getRandomFloat();
                float green = 0.5f + 0.1f * getRandomFloat();
                float blue = 0.7f + 0.1f * getRandomFloat();
                blocks_.back().setColor(brightness * red, brightness * green, brightness * blue);
            }
        }
    }

    void Game::initDungeon()
    {
        dungeonGenerator_.generate();
        for (int i = 0; i < dungeonGenerator_.getRoomBoxCount(); ++i) {
            dig(dungeonGenerator_.getRoomBox(i));
        }
        for (int i = 0; i < dungeonGenerator_.getCorridorBoxCount(); ++i) {
            dig(dungeonGenerator_.getCorridorBox(i));
        }
    }

    void Game::initMonsters()
    {
        monsters_.clear();
        if (dungeonGenerator_.getRoomBoxCount()) {
            Vector2 position = dungeonGenerator_.getRoomBox(0).getCenter();
            monsters_.push_back(new Monster(this, position));
        }
    }

    void Game::initChains()
    {
        chains_.clear();
        if (dungeonGenerator_.getRoomBoxCount()) {
            Box2 roomBox = dungeonGenerator_.getRoomBox(0);
            Vector2 position(roomBox.getCenter().x, roomBox.p2.y);
            chains_.push_back(new Chain(this, position, 20));
        }
    }
                
    void Game::run()
    {
        while (!quit_) {
            double newAppTime = 0.001 * double(SDL_GetTicks());
            if (0.1 < newAppTime - appTime_) {
                appTime_ = newAppTime;
            }

            if (config_->fps) {
                double dt = 1.0 / double(config_->fps);
                while (dt < float(newAppTime - appTime_)) {
                    runStep(float(dt));
                }
            } else {
                runStep(float(newAppTime - appTime_));
            }
        }
    }

    void Game::runStep(float dt)
    {
        appTime_ += dt;
        time_ += dt;
        
        updateFps();
        handleEvents();
        handleInput();
        step(float(dt));
        redraw();
    }
    
    void Game::updateFps()
    {
        if (fpsTime_ < appTime_) {
            char buffer[64];
            sprintf(buffer, "%g FPS", float(fpsCount_));
            fpsText_ = buffer;
            fpsTime_ = appTime_ + 1.0f;
            fpsCount_ = 0;
        }

        ++fpsCount_;
    }
    
    void Game::handleEvents()
    {
        SDL_Event event;
#if 1
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
                initBlocks();
                initDungeon();
                initMonsters();
                initChains();
                break;

            case SDLK_1:
                debugDrawEnabled_ = !debugDrawEnabled_;
                break;

            case SDLK_2:
                lightingEnabled_ = !lightingEnabled_;
                break;

            case SDLK_PLUS:
                cameraScale_ *= config_->cameraZoom;
                break;

            case SDLK_MINUS:
                cameraScale_ /= config_->cameraZoom;
                break;

            case SDLK_UP:
                cameraPosition_.y += config_->cameraPan / cameraScale_;
                break;

            case SDLK_LEFT:
                cameraPosition_.x -= config_->cameraPan / cameraScale_;
                break;

            case SDLK_RIGHT:
                cameraPosition_.x += config_->cameraPan / cameraScale_;
                break;

            case SDLK_DOWN:
                cameraPosition_.y -= config_->cameraPan / cameraScale_;
                break;
        }
    }

    void Game::handleMouseButtonDownEvent(SDL_Event *event)
    {
        if (event->type == SDL_MOUSEBUTTONDOWN) {
            float invScale = 2.0f / cameraScale_ / float(windowHeight_);
            float x = cameraPosition_.x + invScale * float(event->button.x - windowWidth_ / 2);
            float y = cameraPosition_.y + invScale * -float(event->button.y - windowHeight_ / 2);
            Vector2 point(x, y);
            for (BlockIterator i = blocks_.begin(); i != blocks_.end(); ++i) {
                if (getSquaredDistance(i->getPosition(), point) < square(2.0f)) {
                    i->makeDynamic();
                }
            }
        }
    }
    
    void Game::handleInput()
    {
        int x = 0;
        int y = 0;
        Uint8 buttons = SDL_GetMouseState(&x, &y);
        (void) buttons;
        if (!monsters_.empty()) {
            float invScale = 2.0f / cameraScale_ / float(windowHeight_);
            Vector2 targetPosition;
            targetPosition.x = cameraPosition_.x + invScale * float(x - windowWidth_ / 2);
            targetPosition.y = cameraPosition_.y + invScale * -float(y - windowHeight_ / 2);
            monsters_.front().setTargetPosition(targetPosition);
        }

        Uint8 *state = SDL_GetKeyboardState(0);
        if (!monsters_.empty()) {
            bool leftControl = bool(state[SDL_SCANCODE_A]);
            bool rightControl = bool(state[SDL_SCANCODE_D]);
            bool jumpControl = bool(state[SDL_SCANCODE_SPACE]);

            monsters_.front().setLeftControl(leftControl);
            monsters_.front().setRightControl(rightControl);
            monsters_.front().setJumpControl(jumpControl);
        }
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
        for (MonsterIterator i = monsters_.begin(); i != monsters_.end(); ++i) 
        {
            i->stepPhysics(dt);
        }
        physicsWorld_->Step(dt, 10, 10);
        handleCollisions();
        for (MonsterIterator i = monsters_.begin(); i != monsters_.end(); ++i) 
        {
            i->stepAnimation(dt);
        }
        for (BlockIterator i = blocks_.begin(); i != blocks_.end(); ++i) {
            i->makeStaticIfSleeping();
        }
    }

    void Game::handleCollisions()
    { }

    void Game::redraw()
    {
        updateCamera();
        updateFrustum();
        clear();
        draw();
        SDL_GL_SwapWindow(window_);
    }

    void Game::updateCamera()
    {
        if (!monsters_.empty()) {
            Vector2 position = monsters_.front().getPosition();
            cameraPosition_.x = position.x;
            cameraPosition_.y = position.y;
        }
    }
    
    void Game::updateFrustum()
    {
        float invScale = 1.0f / cameraScale_;
        float aspectRatio = float(windowWidth_) / float(windowHeight_);
        frustum_ = Box2(Vector2(cameraPosition_.x - invScale * aspectRatio,
                                cameraPosition_.y - invScale),
                        Vector2(cameraPosition_.x + invScale * aspectRatio,
                                cameraPosition_.y + invScale));
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
            // glEnable(GL_BLEND);
            // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            setLighting();
            drawBlocks();
            if (!monsters_.empty()) {
                monsters_.front().draw();
            }
            if (!chains_.empty()) {
                chains_.front().draw();
            }
            glPopAttrib();
        }
        if (debugDrawEnabled_) {
            glPushAttrib(GL_CURRENT_BIT);
            glColor3f(0.0f, 1.0f, 0.0f);
            physicsWorld_->DrawDebugData();
            // drawBlockBounds();
            // glColor3f(0.0f, 0.5f, 1.0f);
            // delauneyTriangulation_.draw();
            // glColor3f(1.0f, 0.5f, 0.0f);
            // voronoiDiagram_.draw();
            glPopAttrib();
        }
    }

    void Game::setLighting()
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

    void Game::setWorldLight()
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

    void Game::setCameraLight()
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

    void Game::setTargetLight()
    {
        if (!monsters_.empty()) {
            glEnable(GL_LIGHT2);
            GLfloat diffuse[] = { 2.0f, 2.0f, 2.0f, 1.0f };
            GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
            glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
            Vector2 const &targetPosition = monsters_.front().getTargetPosition();
            GLfloat position[] = { targetPosition.x, targetPosition.y, 1.0f, 1.0f };
            glLightfv(GL_LIGHT2, GL_POSITION, position);
            glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0f);
            glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 1.0f);
            glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0f);
        }
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
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(frustum_.p1.x, frustum_.p2.x, frustum_.p1.y, frustum_.p2.y,
                -1.0f, 1.0f);
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
            if (intersects(frustum_, i->getBounds())) {
                i->draw();
            }
        }
    }

    void Game::dig(Box2 const &box)
    {
        for (BlockIterator i = blocks_.begin(); i != blocks_.end(); ++i) {
            if (box.containsPoint(i->getPosition())) {
                int j = i - blocks_.begin();
                blocks_.erase(i);
                i = blocks_.begin() + j - 1;
            }
        }
    }
}
