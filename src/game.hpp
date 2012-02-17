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
    class AnimationService;
    class Config;
    class ControlService;
    class Font;
    class PhysicsService;
    class SceneService;

    class Game {
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

        PhysicsService *getPhysicsService()
        {
            return physicsService_.get();
        }

        ControlService *getControlService()
        {
            return controlService_.get();
        }

        AnimationService *getAnimationService()
        {
            return animationService_.get();
        }

        SceneService *getSceneService()
        {
            return sceneService_.get();
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

        bool blockGrowthDone_;
        bool dungeonGenerationDone_;
        
        Box2 bounds_;

        double fpsTime_;
        int fpsCount_;
        std::string fpsText_;
        
        DelauneyTriangulation delauneyTriangulation_;
        VoronoiDiagram voronoiDiagram_;
        DungeonGenerator dungeonGenerator_;

        std::auto_ptr<PhysicsService> physicsService_;
        std::auto_ptr<ControlService> controlService_;
        std::auto_ptr<SceneService> sceneService_;
        std::auto_ptr<AnimationService> animationService_;
        std::auto_ptr<ActorFactory> actorFactory_;

        ActorVector actors_;
        Actor *playerActor_;

        void initWindow();
        void initContext();
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
    };
}

#endif
