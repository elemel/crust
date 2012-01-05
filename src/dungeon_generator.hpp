#ifndef CRUST_DUNGEON_GENERATOR_HPP
#define CRUST_DUNGEON_GENERATOR_HPP

#include "box.hpp"

#include <vector>

namespace crust {
    class Random;

    class DungeonGenerator {
    public:
        DungeonGenerator(Random *random, Box const &bounds);
        
        void generate();

        int getRoomBoxCount() const
        {
            return int(roomBoxes_.size());
        }

        Box const &getRoomBox(int index) const
        {
            return roomBoxes_[index];
        }

        int getCorridorBoxCount() const
        {
            return int(corridorBoxes_.size());
        }
        
        Box const &getCorridorBox(int index) const
        {
            return corridorBoxes_[index];
        }

    private:
        Random *random_;
        Box bounds_;
        float minRoomSize_;
        float maxRoomSize_;
        float wallSize_;
        float corridorWidth_;
        float corridorHeight_;
        std::vector<Box> roomBoxes_;
        std::vector<Box> corridorBoxes_;

        void generateRooms();
        void generateRoom();
        void generateCorridors();
        bool generateCorridor(Box const &a, Box const &b);
        int intersectsRoom(Box const &box);
    };
}

#endif
