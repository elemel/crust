#ifndef CRUST_DUNGEON_GENERATOR_HPP
#define CRUST_DUNGEON_GENERATOR_HPP

#include "geometry.hpp"

#include <vector>

namespace crust {
    class Random;

    class DungeonGenerator {
    public:
        DungeonGenerator(Random *random, Box2 const &bounds);
        
        void generate();

        int getRoomBoxCount() const
        {
            return int(roomBoxes_.size());
        }

        Box2 const &getRoomBox(int index) const
        {
            return roomBoxes_[index];
        }

        int getCorridorBoxCount() const
        {
            return int(corridorBoxes_.size());
        }
        
        Box2 const &getCorridorBox(int index) const
        {
            return corridorBoxes_[index];
        }

    private:
        Random *random_;
        Box2 bounds_;
        float minRoomSize_;
        float maxRoomSize_;
        float wallSize_;
        float corridorWidth_;
        float corridorHeight_;
        std::vector<Box2> roomBoxes_;
        std::vector<Box2> corridorBoxes_;

        void generateRooms();
        void generateRoom();
        void generateCorridors();
        bool generateCorridor(Box2 const &a, Box2 const &b);
        int intersectsRoom(Box2 const &box);
    };
}

#endif
