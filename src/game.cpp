#include "game.hpp"

#include "actor.hpp"
#include "actor_factory.hpp"
#include "block_physics_component.hpp"
#include "config.hpp"
#include "control_service.hpp"
#include "convert.hpp"
#include "dungeon_generator.hpp"
#include "error.hpp"
#include "geometry.hpp"
#include "graphics_manager.hpp"
#include "input_manager.hpp"
#include "monster_control_component.hpp"
#include "monster_physics_component.hpp"
#include "physics_manager.hpp"

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
        quitting_(false),
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
        inputManager_.reset(new InputManager(this));
        physicsManager_.reset(new PhysicsManager(this));
        controlService_.reset(new ControlService(this));
        graphicsManager_.reset(new GraphicsManager(this));
        initBlocks();
        initDungeon();
        initMonsters();
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
        glClearColor(double(0x33) / 255.0, double(0x33) / 255.0, double(0x33) / 255.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        while (!quitting_) {
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

    void Game::runStep(float dt)
    {
        appTime_ += dt;
        time_ += dt;
        
        updateFps();
        step(float(dt));
        updateCamera();

        SDL_GL_SwapWindow(window_);
        glClearColor(double(0x33) / 255.0, double(0x33) / 255.0, double(0x33) / 255.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        graphicsManager_->draw();
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
        MonsterPhysicsComponent *physicsComponent = convert(playerActor_->getPhysicsComponent());
        b2Vec2 position = physicsComponent->getMainBody()->GetPosition();
        graphicsManager_->setCameraPosition(Vector2(position.x, position.y));
    }
    
    void Game::step(float dt)
    {
        inputManager_->step(dt);
        controlService_->step(dt);
        physicsManager_->step(dt);
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
        graphicsManager_->step(dt);
    }

    void Game::handleCollisions()
    { }

    void Game::removeBlocks(Box2 const &box)
    {
        for (ActorIterator i = actors_.begin(); i != actors_.end(); ++i) {
            Actor *actor = &*i;
            if (isBlock(actor)) {
                BlockPhysicsComponent *physicsComponent = convert(actor->getPhysicsComponent());
                b2Vec2 position = physicsComponent->getBody()->GetPosition();
                if (box.containsPoint(Vector2(position.x, position.y))) {
                    int j = i - actors_.begin();
                    removeActor(actor);
                    i = actors_.begin() + j - 1;
                }
            }
        }
    }
}
