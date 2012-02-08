#include "game.hpp"

#include "block.hpp"
#include "chain.hpp"
#include "config.hpp"
#include "dungeon_generator.hpp"
#include "error.hpp"
#include "geometry.hpp"
#include "monster.hpp"
#include "physics_draw.hpp"
#include "render_manager.hpp"

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

        bounds_(Vector2(-15.0f, -15.0f), Vector2(15.0f, 15.0f)),

        fpsTime_(0.0),
        fpsCount_(0),
    
        delauneyTriangulation_(bounds_),
        dungeonGenerator_(&random_, bounds_),

        mode_(DIG_MODE),
        liftedBlock_(0),
        liftJoint_(0),
        liftTime_(0.0)
    { }
    
    Game::~Game()
    {
        releaseBlock();
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
        initVoronoiDiagram();
        initBlocks();
        initDungeon();
        initMonsters();
        initChains();
        renderManager_.reset(new RenderManager(this));
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
            removeBlocks(dungeonGenerator_.getRoomBox(i));
        }
        for (int i = 0; i < dungeonGenerator_.getCorridorBoxCount(); ++i) {
            removeBlocks(dungeonGenerator_.getCorridorBox(i));
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
        updateCamera();
        renderManager_->redraw();
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
        if (!monsters_.empty() && liftedBlock_ == 0) {
            Vector2 position = monsters_.front().getPosition();
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
                releaseBlock();
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
                mode_ = LIFT_MODE;
                break;

            case SDLK_3:
                mode_ = COLLAPSE_MODE;
                break;
                
            case SDLK_PLUS:
                if (liftedBlock_ == 0) {
                    float scale = renderManager_->getCameraScale();
                    scale *= config_->cameraZoom;
                    renderManager_->setCameraScale(scale);
                }
                break;

            case SDLK_MINUS:
                if (liftedBlock_ == 0) {
                    float scale = renderManager_->getCameraScale();
                    scale /= config_->cameraZoom;
                    renderManager_->setCameraScale(scale);
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
        int x = 0;
        int y = 0;
        Uint8 buttons = SDL_GetMouseState(&x, &y);
        if (!monsters_.empty()) {
            Vector2 targetPosition = renderManager_->getWorldPosition(Vector2(float(x), float(y)));
            monsters_.front().setTargetPosition(targetPosition);
            if (liftedBlock_) {
                liftJoint_->SetTarget(b2Vec2(targetPosition.x, targetPosition.y));
            }
            Uint8 *state = SDL_GetKeyboardState(0);
            if (state[SDL_SCANCODE_LSHIFT] ||
                (buttons & SDL_BUTTON_LMASK) != 0)
            {
                if (mode_ == DIG_MODE) {
                    digBlock(targetPosition);
                } else if (mode_ == COLLAPSE_MODE) {
                    collapseBlocks(targetPosition, 2.0f);
                }
            }
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
            if (&*i != liftedBlock_) {
                b2Body *body = i->getPhysicsBody();
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
        for (BlockIterator i = blocks_.begin(); i != blocks_.end(); ++i) {
            if (box.containsPoint(i->getPosition())) {
                int j = i - blocks_.begin();
                blocks_.erase(i);
                i = blocks_.begin() + j - 1;
            }
        }
    }

    namespace {
        class DigCallback : public b2RayCastCallback {
        public:
            Block *block;

            DigCallback() :
                block(0)
            { }

            float32 ReportFixture(b2Fixture *fixture, b2Vec2 const &point,
                                  b2Vec2 const &normal, float32 fraction)
            {
                b2Body *body = fixture->GetBody();
                Actor *actor = static_cast<Actor *>(body->GetUserData());
                Block *tempBlock = dynamic_cast<Block *>(actor);
                if (tempBlock) {
                    block = tempBlock;
                    return fraction;
                } else {
                    return 1.0f;
                }
            }
        };
    }
    
    void Game::digBlock(Vector2 const &point)
    {
        if (!monsters_.empty()) {
            Vector2 p1 = monsters_.front().getPosition();
            Vector2 p2 = p1 + clampLength(point - p1, 1.5f);
            DigCallback callback;
            physicsWorld_->RayCast(&callback, b2Vec2(p1.x, p1.y),
                                   b2Vec2(p2.x, p2.y));
            if (callback.block) {
                for (BlockIterator i = blocks_.begin(); i != blocks_.end(); ++i) {
                    if (&*i == callback.block) {
                        blocks_.erase(i);
                        break;
                    }
                }
            }
        }
    }

    void Game::liftBlock(Vector2 const &point)
    {
        if (liftedBlock_) {
            return;
        }

        Block *block = 0;
        for (BlockIterator i = blocks_.begin(); i != blocks_.end(); ++i) {
            if (i->containsPoint(point)) {
                block = &*i;
            }
        }

        if (block) {
            b2Body *body = block->getPhysicsBody();

            b2Vec2 localPointVec2 = body->GetLocalPoint(b2Vec2(point.x, point.y));
            Vector2 localPoint(localPointVec2.x, localPointVec2.y);
            Polygon2 const &localPolygon = block->getLocalPolygon();
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

            liftedBlock_ = block;

            b2MouseJointDef liftJointDef;
            liftJointDef.target.Set(point.x, point.y);
            liftJointDef.bodyA = body;
            liftJointDef.bodyB = body;
            liftJointDef.maxForce = 5.0f * body->GetMass() * 10.0f;
            liftJoint_ = static_cast<b2MouseJoint *>(physicsWorld_->CreateJoint(&liftJointDef));

            liftTime_ = time_;
        }
    }

    void Game::releaseBlock()
    {
        if (liftedBlock_) {
            b2Body *body = liftedBlock_->getPhysicsBody();
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

            physicsWorld_->DestroyJoint(liftJoint_);
            liftJoint_ = 0;
            liftedBlock_ = 0;
        }
    }

    void Game::collapseBlocks(Vector2 const &point, float distance)
    {
        for (BlockIterator i = blocks_.begin(); i != blocks_.end(); ++i) {
            if (getSquaredDistance(i->getPosition(), point) < square(distance)) {
                i->getPhysicsBody()->SetType(b2_dynamicBody);
            }
        }
    }
}
