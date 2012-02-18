#include "game.hpp"

#include "actor.hpp"
#include "actor_factory.hpp"
#include "animation_component.hpp"
#include "animation_service.hpp"
#include "block_physics_component.hpp"
#include "config.hpp"
#include "control_component.hpp"
#include "control_service.hpp"
#include "convert.hpp"
#include "dungeon_generator.hpp"
#include "error.hpp"
#include "geometry.hpp"
#include "monster_control_component.hpp"
#include "physics_component.hpp"
#include "physics_service.hpp"
#include "scene_service.hpp"

#include <fstream>

namespace crust {
    namespace {
        bool isBlock(Actor const *actor)
        {
            return dynamic_cast<BlockPhysicsComponent const *>(actor->getPhysicsComponent());
        }
    }

    Game::Game(Config const *config) :
        config_(config),
        quit_(false),
        windowWidth_(config->windowWidth),
        windowHeight_(config->windowHeight),
        window_(0),
        context_(0),

        appTime_(0.0),
        time_(0.0),

        bounds_(Vector2(-15.0f, -15.0f), Vector2(15.0f, 15.0f)),

        fpsTime_(0.0),
        fpsCount_(0),
    
        delauneyTriangulation_(bounds_),
        dungeonGenerator_(&random_, bounds_),

        playerActor_(0)
    {
        actorFactory_.reset(new ActorFactory(this));
        initWindow();
        initContext();
        initVoronoiDiagram();
        physicsService_.reset(new PhysicsService(this));
        controlService_.reset(new ControlService(this));
        sceneService_.reset(new SceneService(this));
        animationService_.reset(new AnimationService(this));
        initBlocks();
        initDungeon();
        initMonsters();
        initChains();
    }
    
    Game::~Game()
    {
        while (!actors_.empty()) {
            actors_.back().destroy();
            actors_.pop_back();
        }
        if (context_) {
            SDL_GL_DeleteContext(context_);
        }
        if (window_) {
            SDL_DestroyWindow(window_);
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
    
    float Game::getRandomFloat()
    {
        return random_.getFloat();
    }

    int Game::getRandomInt(int size)
    {
        return random_.getInt(size);
    }

    Actor *Game::addActor(std::auto_ptr<Actor> actor)
    {
        actors_.push_back(actor);
        actors_.back().create();
        return &actors_.back();
    }

    void Game::removeActor(Actor *actor)
    {
        ActorVector::auto_type removedActor;
        for (ActorIterator i = actors_.begin(); i != actors_.end(); ++i) {
            if (&*i == actor) {
                i->destroy();
                actors_.erase(i);
                break;
            }
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
    
    void Game::initVoronoiDiagram()
    {
        Box2 vertexBounds = bounds_;
        vertexBounds.pad(5.0f);
        Box2 triangulationBounds = vertexBounds;
        triangulationBounds.pad(5.0f);
        delauneyTriangulation_ = DelauneyTriangulation(triangulationBounds);
        int subdivCount = 30;
        float subdivWidth = float(vertexBounds.getWidth()) / float(subdivCount);
        float subdivHeight = float(vertexBounds.getHeight()) / float(subdivCount);
        for (int i = 0; i < subdivCount; ++i) {
            for (int j = 0; j < subdivCount; ++j) {
                float x = vertexBounds.p1.x + (float(i) + getRandomFloat()) * subdivWidth;
                float y = vertexBounds.p1.y + (float(j) + getRandomFloat()) * subdivHeight;
                delauneyTriangulation_.addVertex(Vector2(x, y));
            }
        }
        voronoiDiagram_.generate(delauneyTriangulation_);
    }

    void Game::initBlocks()
    {
        Box2 paddedBounds = bounds_;
        paddedBounds.pad(2.0f);
        for (int i = 0; i < voronoiDiagram_.getPolygonCount(); ++i) {
            Polygon2 polygon = voronoiDiagram_.getPolygon(i);
            if (contains(paddedBounds, polygon)) {
                addActor(actorFactory_->createBlock(polygon));
            }
        }
    }

    void Game::initDungeon()
    {
        dungeonGenerator_.generate();
        for (int i = 0; i < dungeonGenerator_.getRoomBoxCount(); ++i) {
            removeBlocks(dungeonGenerator_.getRoomBox(i));
        }
        for (int i = 0; i < dungeonGenerator_.getCorridorBoxCount(); ++i) {
            removeBlocks(dungeonGenerator_.getCorridorBox(i));
        }
    }

    void Game::initMonsters()
    {
        if (dungeonGenerator_.getRoomBoxCount()) {
            Vector2 position = dungeonGenerator_.getRoomBox(0).getCenter();
            playerActor_ = addActor(actorFactory_->createMonster(position));
        }
    }

    void Game::initChains()
    {
        if (dungeonGenerator_.getRoomBoxCount()) {
            Box2 roomBox = dungeonGenerator_.getRoomBox(0);
            Vector2 position(roomBox.getCenter().x, roomBox.p2.y);
            addActor(actorFactory_->createChain(position, 20));
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
        updateCamera();

        glClearColor(0.2, 0.2, 0.3, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        sceneService_->draw();
        SDL_GL_SwapWindow(window_);
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
    
    void Game::updateCamera()
    {
        Vector2 position = playerActor_->getPhysicsComponent()->getPosition();
        sceneService_->setCameraPosition(position);
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

            case SDL_KEYUP:
                handleKeyUpEvent(event);
                break;

            case SDL_MOUSEBUTTONDOWN:
                handleMouseButtonDownEvent(event);
                break;

            case SDL_MOUSEBUTTONUP:
                handleMouseButtonUpEvent(event);
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
                playerActor_ = 0;
                while (!actors_.empty()) {
                    actors_.back().destroy();
                    actors_.pop_back();
                }
                initVoronoiDiagram();
                initBlocks();
                initDungeon();
                initMonsters();
                // initChains();
                break;

            case SDLK_1:
                if (playerActor_) {
                    MonsterControlComponent *controlComponent = convert(playerActor_->getControlComponent());
                    controlComponent->setActionMode(MonsterControlComponent::MINE_MODE);
                }
                break;

            case SDLK_2:
                if (playerActor_) {
                    MonsterControlComponent *controlComponent = convert(playerActor_->getControlComponent());
                    controlComponent->setActionMode(MonsterControlComponent::DRAG_MODE);
                }
                break;

            case SDLK_3:
                if (playerActor_) {
                    MonsterControlComponent *controlComponent = convert(playerActor_->getControlComponent());
                    controlComponent->setActionMode(MonsterControlComponent::DROP_MODE);
                }
                break;

            case SDLK_PLUS:
                {
                    float scale = sceneService_->getCameraScale();
                    scale *= config_->cameraZoom;
                    if (scale < config_->maxCameraScale) {
                        sceneService_->setCameraScale(scale);
                    }
                }
                break;

            case SDLK_MINUS:
                {
                    float scale = sceneService_->getCameraScale();
                    scale /= config_->cameraZoom;
                    if (scale > config_->minCameraScale) {
                        sceneService_->setCameraScale(scale);
                    }
                }
                break;

            case SDLK_UP:
                // cameraPosition_.y += config_->cameraPan / cameraScale_;
                break;

            case SDLK_LEFT:
                // cameraPosition_.x -= config_->cameraPan / cameraScale_;
                break;

            case SDLK_RIGHT:
                // cameraPosition_.x += config_->cameraPan / cameraScale_;
                break;

            case SDLK_DOWN:
                // cameraPosition_.y -= config_->cameraPan / cameraScale_;
                break;
        }
    }

    void Game::handleKeyUpEvent(SDL_Event *event)
    { }

    void Game::handleMouseButtonDownEvent(SDL_Event *event)
    { }

    void Game::handleMouseButtonUpEvent(SDL_Event *event)
    { }

    void Game::handleInput()
    {
        if (playerActor_) {
            ControlComponent *controlComponent = playerActor_->getControlComponent();

            int x = 0;
            int y = 0;
            Uint8 mouseButtons = SDL_GetMouseState(&x, &y);
            Uint8 *keyboardState = SDL_GetKeyboardState(0);

            bool leftControl = bool(keyboardState[SDL_SCANCODE_A]);
            bool rightControl = bool(keyboardState[SDL_SCANCODE_D]);
            bool jumpControl = bool(keyboardState[SDL_SCANCODE_SPACE]);
            bool actionControl = bool(keyboardState[SDL_SCANCODE_LSHIFT] || (mouseButtons & SDL_BUTTON_LMASK));
            Vector2 targetPosition = sceneService_->getWorldPosition(Vector2(float(x), float(y)));

            controlComponent->setLeftControl(leftControl);
            controlComponent->setRightControl(rightControl);
            controlComponent->setJumpControl(jumpControl);
            controlComponent->setActionControl(actionControl);
            controlComponent->setTargetPosition(targetPosition);
        }
    }
    
    void Game::step(float dt)
    {
        controlService_->step(dt);
        physicsService_->step(dt);
        handleCollisions();
        for (ActorIterator i = actors_.begin(); i != actors_.end(); ++i) {
            Actor *actor = &*i;
            if (isBlock(actor)) {
                b2Body *body = static_cast<BlockPhysicsComponent *>(actor->getPhysicsComponent())->getBody();
                if (body->GetType() != b2_staticBody && !body->IsAwake()) {
                    body->SetType(b2_staticBody);
                }
            }
        }
        animationService_->step(dt);
        sceneService_->step(dt);
    }

    void Game::handleCollisions()
    { }

    void Game::removeBlocks(Box2 const &box)
    {
        for (ActorIterator i = actors_.begin(); i != actors_.end(); ++i) {
            Actor *actor = &*i;
            if (isBlock(actor) && box.containsPoint(i->getPhysicsComponent()->getPosition())) {
                int j = i - actors_.begin();
                removeActor(actor);
                i = actors_.begin() + j - 1;
            }
        }
    }
}
