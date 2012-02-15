#include "game.hpp"

#include "actor.hpp"
#include "actor_factory.hpp"
#include "animation_component.hpp"
#include "block_physics_component.hpp"
#include "config.hpp"
#include "control_component.hpp"
#include "dungeon_generator.hpp"
#include "error.hpp"
#include "geometry.hpp"
#include "physics_component.hpp"
#include "physics_draw.hpp"
#include "render_manager.hpp"

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

        playerActor_(0),
    
        bounds_(Vector2(-15.0f, -15.0f), Vector2(15.0f, 15.0f)),

        fpsTime_(0.0),
        fpsCount_(0),
    
        delauneyTriangulation_(bounds_),
        dungeonGenerator_(&random_, bounds_),

        mode_(DIG_MODE),
        liftedBlock_(0),
        liftJoint_(0),
        liftTime_(0.0)
    {
        actorFactory_.reset(new ActorFactory(this));
        initWindow();
        initContext();
        initPhysics();
        initVoronoiDiagram();
        initBlocks();
        initDungeon();
        initMonsters();
        initChains();
        renderManager_.reset(new RenderManager(this));
    }
    
    Game::~Game()
    {
        releaseBlock();
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
    
    void Game::initPhysics()
    {
        b2Vec2 gravity(0.0f, -10.0f);
        physicsWorld_.reset(new b2World(gravity));
        physicsWorld_->SetContactListener(this);
        physicsDraw_.reset(new PhysicsDraw);
        physicsWorld_->SetDebugDraw(physicsDraw_.get());
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
        renderManager_->draw();
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
        if (playerActor_ && liftedBlock_ == 0) {
            Vector2 position = playerActor_->getPhysicsComponent()->getPosition();
            renderManager_->setCameraPosition(position);
        }
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
                releaseBlock();
                while (!actors_.empty()) {
                    actors_.back().destroy();
                    actors_.pop_back();
                }
                initVoronoiDiagram();
                initBlocks();
                initDungeon();
                initMonsters();
                initChains();
                break;

            case SDLK_1:
                mode_ = DIG_MODE;
                break;

            case SDLK_2:
                mode_ = CHAIN_MODE;
                break;

            case SDLK_3:
                mode_ = LIFT_MODE;
                break;

            case SDLK_4:
                mode_ = COLLAPSE_MODE;
                break;

            case SDLK_PLUS:
                if (liftedBlock_ == 0) {
                    float scale = renderManager_->getCameraScale();
                    scale *= config_->cameraZoom;
                    if (scale < config_->maxCameraScale) {
                        renderManager_->setCameraScale(scale);
                    }
                }
                break;

            case SDLK_MINUS:
                if (liftedBlock_ == 0) {
                    float scale = renderManager_->getCameraScale();
                    scale /= config_->cameraZoom;
                    if (scale > config_->minCameraScale) {
                        renderManager_->setCameraScale(scale);
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

            case SDLK_LSHIFT:
            {
                int x = 0;
                int y = 0;
                SDL_GetMouseState(&x, &y);
                Vector2 targetPosition = renderManager_->getWorldPosition(Vector2(float(x), float(y)));
                if (mode_ == LIFT_MODE) {
                    liftBlock(targetPosition);
                }
                break;
            }
        }
    }

    void Game::handleKeyUpEvent(SDL_Event *event)
    {
        switch (event->key.keysym.sym) {
            case SDLK_LSHIFT:
                releaseBlock();
                break;
        }
    }

    void Game::handleMouseButtonDownEvent(SDL_Event *event)
    {
        if (mode_ == LIFT_MODE) {
            Vector2 screenPosition(float(event->button.x), float(event->button.y));
            Vector2 position = renderManager_->getWorldPosition(screenPosition);
            liftBlock(position);
        }
    }

    void Game::handleMouseButtonUpEvent(SDL_Event *event)
    {
        Uint8 *state = SDL_GetKeyboardState(0);
        if (!state[SDL_SCANCODE_LSHIFT]) {
            releaseBlock();
        }
    }

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
            Vector2 targetPosition = renderManager_->getWorldPosition(Vector2(float(x), float(y)));

            controlComponent->setLeftControl(leftControl);
            controlComponent->setRightControl(rightControl);
            controlComponent->setJumpControl(jumpControl);
            controlComponent->setActionControl(actionControl);
            controlComponent->setTargetPosition(targetPosition);

            if (actionControl) {
                if (mode_ == COLLAPSE_MODE) {
                    collapseBlocks(targetPosition, 2.0f);
                }
            }
            if (liftedBlock_) {
                liftJoint_->SetTarget(b2Vec2(targetPosition.x, targetPosition.y));
            }
        }
    }
    
    void Game::step(float dt)
    {
        for (ActorIterator i = actors_.begin(); i != actors_.end(); ++i) {
            if (i->getControlComponent()) {
                i->getControlComponent()->step(dt);
            }
        }
        physicsWorld_->Step(dt, 10, 10);
        handleCollisions();
        for (ActorIterator i = actors_.begin(); i != actors_.end(); ++i) {
            if (i->getAnimationComponent()) {
                i->getAnimationComponent()->step(dt);
            }
        }
        for (ActorIterator i = actors_.begin(); i != actors_.end(); ++i) {
            Actor *actor = &*i;
            if (isBlock(actor) && actor != liftedBlock_) {
                b2Body *body = static_cast<BlockPhysicsComponent *>(actor->getPhysicsComponent())->getBody();
                if (body->GetType() != b2_staticBody && !body->IsAwake()) {
                    body->SetType(b2_staticBody);
                }
            }
        }
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
    
    void Game::liftBlock(Vector2 const &point)
    {
        if (liftedBlock_) {
            return;
        }

        Actor *actor = 0;
        for (ActorIterator i = actors_.begin(); i != actors_.end(); ++i) {
            if (isBlock(&*i) && static_cast<BlockPhysicsComponent *>(i->getPhysicsComponent())->containsPoint(point)) {
                actor = &*i;
            }
        }

        if (actor) {
            b2Body *body = static_cast<BlockPhysicsComponent *>(actor->getPhysicsComponent())->getBody();

            b2Vec2 localPointVec2 = body->GetLocalPoint(b2Vec2(point.x, point.y));
            Vector2 localPoint(localPointVec2.x, localPointVec2.y);
            Polygon2 const &localPolygon = static_cast<BlockPhysicsComponent *>(actor->getPhysicsComponent())->getLocalPolygon();
            Vector2 localCentroid = localPolygon.getCentroid();
            float squaredCentroidDistance = getSquaredDistance(localPoint, localCentroid);
            bool fixedRotation = true;
            for (std::size_t i = 0; i < localPolygon.vertices.size(); ++i) {
                if (getSquaredDistance(localPoint, localPolygon.vertices[i]) < squaredCentroidDistance) {
                    fixedRotation = false;
                }
            }

            if (fixedRotation) {
                body->SetAngularVelocity(0.0f);
                body->SetFixedRotation(true);
            }
            body->SetType(b2_dynamicBody);
            body->SetGravityScale(0.0f);

            liftedBlock_ = actor;

            b2MouseJointDef liftJointDef;
            liftJointDef.target.Set(point.x, point.y);
            liftJointDef.bodyA = body;
            liftJointDef.bodyB = body;
            liftJointDef.maxForce = 1.0f * body->GetMass() * 10.0f;
            liftJoint_ = static_cast<b2MouseJoint *>(physicsWorld_->CreateJoint(&liftJointDef));

            liftTime_ = time_;
        }
    }

    void Game::releaseBlock()
    {
        if (liftedBlock_) {
            b2Body *body = static_cast<BlockPhysicsComponent *>(liftedBlock_->getPhysicsComponent())->getBody();
            bool makeStatic = false;
            b2Vec2 linearVelocity = body->GetLinearVelocityFromWorldPoint(liftJoint_->GetTarget());
            float angularVelocity = body->GetAngularVelocity();
            if (linearVelocity.LengthSquared() < square(0.1f) &&
                std::abs(angularVelocity) < 0.1f)
            {
                for (b2ContactEdge *i = body->GetContactList(); i; i = i->next) {
                    if (i->contact->IsTouching() && i->other->GetType() == b2_staticBody) {
                        makeStatic = true;
                    }
                }
            }
            if (makeStatic) {
                body->SetType(b2_staticBody);
            }
            body->SetFixedRotation(false);
            body->SetGravityScale(1.0f);

            physicsWorld_->DestroyJoint(liftJoint_);
            liftJoint_ = 0;
            liftedBlock_ = 0;
        }
    }

    void Game::collapseBlocks(Vector2 const &point, float distance)
    {
        for (ActorIterator i = actors_.begin(); i != actors_.end(); ++i) {
            if (isBlock(&*i) && getSquaredDistance(i->getPhysicsComponent()->getPosition(), point) < square(distance)) {
                static_cast<BlockPhysicsComponent *>(i->getPhysicsComponent())->getBody()->SetType(b2_dynamicBody);
            }
        }
    }
}
