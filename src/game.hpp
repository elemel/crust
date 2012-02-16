#ifndef CRUST_GAME_HPP
#define CRUST_GAME_HPP

#include "delauney_triangulation.hpp"
#include "dungeon_generator.hpp"
#include "geometry.hpp"
#include "random.hpp"
#include "voronoi_diagram.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <boost/ptr_container/ptr_vector.hpp>
#include <Box2D/Box2D.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

namespace crust {
    class Actor;
    class ActorFactory;
    class Config;
    class Font;
    class RenderManager;

    class Game : public b2ContactListener {
    public:
        typedef boost::ptr_vector<Actor> ActorVector;
        typedef ActorVector::iterator ActorIterator;
        typedef ActorVector::const_iterator ConstActorIterator;

        explicit Game(Config const *config);
        ~Game();
        
        void run();

        Config const *getConfig() const
        {
            return config_;
        }

        SDL_Window *getWindow()
        {
            return window_;
        }
        
        double getTime() const
        {
            return time_;
        }
        
        b2World *getPhysicsWorld()
        {
            return physicsWorld_.get();
        }

        void BeginContact(b2Contact* contact);
        void EndContact(b2Contact* contact);

        float getRandomFloat();
        int getRandomInt(int size);

        ActorVector const &getActors() const
        {
            return actors_;
        }

        Actor *getPlayerActor()
        {
            return playerActor_;
        }

        Actor const *getPlayerActor() const
        {
            return playerActor_;
        }
        
        const char *getFpsText() const
        {
            return fpsText_.c_str();
        }

        Actor *addActor(std::auto_ptr<Actor> actor);
        void removeActor(Actor *actor);

        int getActorCount() const
        {
            return int(actors_.size());
        }

        Actor *getActor(int i)
        {
            return &actors_[i];
        }

        Actor const *getActor(int i) const
        {
            return &actors_[i];
        }

    private:
        Config const *config_;
        Random random_;
        bool quit_;
        int windowWidth_;
        int windowHeight_;
        SDL_Window *window_;
        SDL_GLContext context_;

        double appTime_;
        double time_;

        std::auto_ptr<b2World> physicsWorld_;
        std::auto_ptr<b2Draw> physicsDraw_;
        bool blockGrowthDone_;
        bool dungeonGenerationDone_;
        std::vector<Actor *> blocksWithNeighbors_;
        ActorVector actors_;
        Actor *playerActor_;
        
        Box2 bounds_;

        double fpsTime_;
        int fpsCount_;
        std::string fpsText_;
        
        Actor *liftedBlock_;
        b2MouseJoint *liftJoint_;
        double liftTime_;

        DelauneyTriangulation delauneyTriangulation_;
        VoronoiDiagram voronoiDiagram_;
        DungeonGenerator dungeonGenerator_;

        std::auto_ptr<RenderManager> renderManager_;
        std::auto_ptr<ActorFactory> actorFactory_;
        
        void initWindow();
        void initContext();
        void initPhysics();
        void initVoronoiDiagram();
        void initBlocks();
        void initDungeon();
        void initMonsters();
        void initChains();

        void runStep(float dt);
        void updateFps();
        void updateCamera();

        void handleEvents();
        void handleEvent(SDL_Event *event);
        void handleKeyDownEvent(SDL_Event *event);
        void handleKeyUpEvent(SDL_Event *event);
        void handleMouseButtonDownEvent(SDL_Event *event);
        void handleMouseButtonUpEvent(SDL_Event *event);
        void handleInput();

        void step(float dt);
        void handleCollisions();
        
        void removeBlocks(Box2 const &box);

        void liftBlock(Vector2 const &point);
        void releaseBlock();
    };
}

#endif
