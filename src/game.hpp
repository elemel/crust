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
    class Block;
    class Config;
    class Font;
    class Monster;
    class TextDrawer;

    class Game : public b2ContactListener {
    public:
        typedef boost::ptr_vector<Block> BlockVector;
        typedef BlockVector::iterator BlockIterator;
        typedef std::map<b2Body *, b2Body *> CollisionMap;
        typedef CollisionMap::iterator CollisionIterator;
        typedef boost::ptr_vector<Monster> MonsterVector;
        typedef MonsterVector::iterator MonsterIterator;

        explicit Game(Config const *config);
        ~Game();
        
        int main(int argc, char **argv);

        float getTime();
        b2World *getPhysicsWorld();

        void BeginContact(b2Contact* contact);
        void EndContact(b2Contact* contact);

        Block *findBlockAtPosition(float x, float y, Block *except);

        float getRandomFloat();
        int getRandomInt(int size);

    private:
        Config const *config_;
        Random random_;
        bool quit_;
        int windowWidth_;
        int windowHeight_;
        SDL_Window *window_;
        SDL_GLContext context_;
        float time_;
        std::auto_ptr<b2World> physicsWorld_;
        std::auto_ptr<b2Draw> physicsDraw_;
        BlockVector blocks_;
        bool blockGrowthDone_;
        bool dungeonGenerationDone_;
        std::vector<Block *> blocksWithNeighbors_;
        CollisionMap particleToBlockCollisions_;
        MonsterVector monsters_;

        float cameraStep_;
        float cameraX_;
        float cameraY_;
        float cameraScale_;

        float targetX_;
        float targetY_;
        
        bool drawEnabled_;
        bool debugDrawEnabled_;
        bool lightingEnabled_;
        std::auto_ptr<Font> font_;        
        std::auto_ptr<TextDrawer> textDrawer_;
        
        Box2 bounds_;

        int fpsTime_;
        int fpsCount_;
        std::string fpsText_;

        DelauneyTriangulation delauneyTriangulation_;
        VoronoiDiagram voronoiDiagram_;
        DungeonGenerator dungeonGenerator_;

        void init();
        void initSdl();
        void initWindow();
        void initContext();
        void initPhysics();
        void initFont();
        void initVoronoiDiagram();
        void initBlocks();
        void initDungeon();
        void initMonsters();

        void run();
        float updateTime();
        void updateFps();

        void handleEvents();
        void handleEvent(SDL_Event *event);
        void handleKeyDownEvent(SDL_Event *event);
        void handleMouseButtonDownEvent(SDL_Event *event);
        void handleInput();
        void setBlockElementAtPosition(float x, float y, int type);

        void step(float dt);
        void stepMonsters(float dt);
        void handleCollisions();
        
        void redraw();
        void clear();
        void draw();
        void drawWorld();
        void setLighting();
        void setWorldLight();
        void setCameraLight();
        void setTargetLight();
        void drawOverlay();
        void drawFps();
        void setWorldProjection();
        void setOverlayProjection();
        void drawBlockBounds();
        void drawBlocks();

        void dig(Box2 const &box);
    };
}

#endif
