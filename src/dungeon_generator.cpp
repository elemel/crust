#include "dungeon_generator.hpp"

#include "random.hpp"

namespace crust {
    DungeonGenerator::DungeonGenerator(Random *random, Box const &bounds) :
        random_(random),
        bounds_(bounds),
        minRoomSize_(3.0f),
        maxRoomSize_(9.0f),
        wallSize_(0.75f),
        corridorWidth_(1.0f),
        corridorHeight_(1.5f)
    {
        bounds_.pad(-wallSize_, -wallSize_);
    }

    void DungeonGenerator::generate()
    {
        generateRooms();
        generateCorridors();
    }

    void DungeonGenerator::generateRooms()
    {
        for (int i = 0; i < 500; ++i) {
            generateRoom();
        }
    }

    void DungeonGenerator::generateRoom()
    {
        float width = minRoomSize_ + random_->getFloat() * (maxRoomSize_ - minRoomSize_);
        float height = minRoomSize_ + random_->getFloat() * (maxRoomSize_ - minRoomSize_);
        float size = 0.5f * width + 0.5f * height;
        width = 0.5f * width + 0.5f * size;
        height = 0.5f * height + 0.5f * size;
        if (width < bounds_.getWidth() && height < bounds_.getHeight()) {
            float x = bounds_.getX() + random_->getFloat() * (bounds_.getWidth() - width);
            float y = bounds_.getY() + random_->getFloat() * (bounds_.getHeight() - height);
            Box box(x, y, width, height);
            if (!intersectsRoom(box)) {
                roomBoxes_.push_back(box);
            }
        }
    }

    void DungeonGenerator::generateCorridors()
    {
        for (std::size_t i = 0; i < roomBoxes_.size(); ++i) {
            for (std::size_t j = 0; j < roomBoxes_.size(); ++j) {
                if (i < j) {
                    generateCorridor(roomBoxes_[i], roomBoxes_[j]);
                }
            }
        }
    }
    
    bool DungeonGenerator::generateCorridor(Box const &a, Box const &b)
    {
        float minX = std::max(a.getX(), b.getX());
        float minY = std::max(a.getY(), b.getY());
        float maxX = std::min(a.getX() + a.getWidth(),
                              b.getX() + b.getWidth());
        float maxY = std::min(a.getY() + a.getHeight(),
                              b.getY() + b.getHeight());
        float width = maxX - minX;
        float height = maxY - minY;
        if (corridorWidth_ < width) {
            Box const &top = a.getY() < b.getY() ? b : a;
            Box const &bottom = a.getY() < b.getY() ? a : b;

            float topMinX = top.getX();
            float topMaxX = top.getX() + top.getWidth();
            float bottomMinX = bottom.getX();
            float bottomMaxX = bottom.getX() + bottom.getWidth();

            bool left = (topMinX < bottomMinX);
            bool right = (bottomMaxX < topMaxX);

            float x = 0.0f;
            if (left && right) {
                x = random_->getInt(2) ? minX : minX + width - corridorWidth_;
            } else if (left) {
                x = minX;
            } else if (right) {
                x = minX + width - corridorWidth_;
            } else {
                x = minX + 0.5f * (width - corridorWidth_);
            }

            Box box(x, maxY, corridorWidth_, -height);
            if (intersectsRoom(box) == 2) {
                corridorBoxes_.push_back(box);
                return true;
            }
        } else if (corridorHeight_ < height) {
            Box box(maxX, minY, -width, corridorHeight_);
            if (intersectsRoom(box) == 2) {
                corridorBoxes_.push_back(box);
                return true;
            }
        }
        return false;
    }

    int DungeonGenerator::intersectsRoom(Box const &box)
    {
        Box paddedBox(box);
        paddedBox.pad(wallSize_, wallSize_);
        int count = 0;
        for (std::size_t i = 0; i < roomBoxes_.size(); ++i) {
            if (intersects(paddedBox, roomBoxes_[i])) {
                ++count;
            }
        }
        return count;
    }
}
