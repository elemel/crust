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
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

namespace crust {
    class Actor;
    class ActorFactory;
    class Config;
    class ControlService;
    class Font;
    class GraphicsManager;
    class InputService;
    class PhysicsService;

    class Game {
    public:
        typedef boost::ptr_vector<Actor> ActorVector;
        typedef ActorVector::iterator ActorIterator;
        typedef ActorVector::const_iterator ConstActorIterator;

        explicit Game(Config const *config);
        ~Game();
        
        void run();

        void setQuitting(bool quitting)
        {
            quitting_ = quitting;
        }
        
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

        InputService *getInputService()
        {
            return inputService_.get();
        }

        PhysicsService *getPhysicsService()
        {
            return physicsService_.get();
        }

        ControlService *getControlService()
        {
            return controlService_.get();
        }

        GraphicsManager *getGraphicsManager()
        {
            return graphicsManager_.get();
        }

        GraphicsManager const *getGraphicsManager() const
        {
            return graphicsManager_.get();
        }

    private:
        Config const *config_;
        Random random_;
        bool quitting_;
        int windowWidth_;
        int windowHeight_;
        SDL_Window *window_;
        SDL_GLContext context_;

        double appTime_;
        double time_;

        bool blockGrowthDone_;
        bool dungeonGenerationDone_;
        
        Box2 bounds_;

        double fpsTime_;
        int fpsCount_;
        std::string fpsText_;
        
        DelauneyTriangulation delauneyTriangulation_;
        VoronoiDiagram voronoiDiagram_;
        DungeonGenerator dungeonGenerator_;

        std::auto_ptr<InputService> inputService_;
        std::auto_ptr<PhysicsService> physicsService_;
        std::auto_ptr<ControlService> controlService_;
        std::auto_ptr<GraphicsManager> graphicsManager_;
        std::auto_ptr<ActorFactory> actorFactory_;

        ActorVector actors_;
        Actor *playerActor_;

        void initWindow();
        void initContext();
        void initVoronoiDiagram();
        void initBlocks();
        void initDungeon();
        void initMonsters();

        void runStep(float dt);
        void updateFps();
        void updateCamera();

        void step(float dt);
        void handleCollisions();
        
        void removeBlocks(Box2 const &box);
    };
}

#endif
