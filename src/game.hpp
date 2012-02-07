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
    class Chain;
    class Config;
    class Font;
    class Monster;
    class TextDrawer;

    class Game : public b2ContactListener {
    public:
        enum Mode {
            DIG_MODE,
            LIFT_MODE,
            COLLAPSE_MODE,

            MODE_COUNT
        };

        typedef boost::ptr_vector<Block> BlockVector;
        typedef BlockVector::iterator BlockIterator;
        typedef std::map<b2Body *, b2Body *> CollisionMap;
        typedef CollisionMap::iterator CollisionIterator;
        typedef boost::ptr_vector<Monster> MonsterVector;
        typedef MonsterVector::iterator MonsterIterator;
        typedef boost::ptr_vector<Chain> ChainVector;
        typedef ChainVector::iterator ChainIterator;
        
        explicit Game(Config const *config);
        ~Game();
        
        int main(int argc, char **argv);

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

        double appTime_;
        double time_;

        std::auto_ptr<b2World> physicsWorld_;
        std::auto_ptr<b2Draw> physicsDraw_;
        BlockVector blocks_;
        bool blockGrowthDone_;
        bool dungeonGenerationDone_;
        std::vector<Block *> blocksWithNeighbors_;
        CollisionMap particleToBlockCollisions_;
        MonsterVector monsters_;
        ChainVector chains_;

        Vector2 cameraPosition_;
        float cameraScale_;
        Box2 frustum_;

        bool drawEnabled_;
        bool debugDrawEnabled_;
        bool lightingEnabled_;
        std::auto_ptr<Font> font_;        
        std::auto_ptr<TextDrawer> textDrawer_;
        
        Box2 bounds_;

        double fpsTime_;
        int fpsCount_;
        std::string fpsText_;
        
        Mode mode_;
        Block *liftedBlock_;
        b2MouseJoint *liftJoint_;
        double liftTime_;

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
        void initChains();

        void run();
        void runStep(float dt);
        void updateFps();

        void handleEvents();
        void handleEvent(SDL_Event *event);
        void handleKeyDownEvent(SDL_Event *event);
        void handleKeyUpEvent(SDL_Event *event);
        void handleMouseButtonDownEvent(SDL_Event *event);
        void handleMouseButtonUpEvent(SDL_Event *event);
        void handleInput();

        void setBlockElementAtPosition(float x, float y, int type);

        void step(float dt);
        void handleCollisions();
        
        void redraw();
        void updateCamera();
        void updateFrustum();
        void clear();
        void draw();
        void drawWorld();
        void setLighting();
        void setWorldLight();
        void setCameraLight();
        void setTargetLight();
        void drawOverlay();
        void drawMode();
        void drawFps();
        void setWorldProjection();
        void setOverlayProjection();
        void drawBlockBounds();
        void drawBlocks();

        void removeBlocks(Box2 const &box);

        void digBlock(Vector2 const &point);
        void liftBlock(Vector2 const &point);
        void releaseBlock();
        void collapseBlocks(Vector2 const &point, float distance);
    };
}

#endif
