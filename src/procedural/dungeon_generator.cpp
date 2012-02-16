#include "dungeon_generator.hpp"

#include "random.hpp"

namespace crust {
    DungeonGenerator::DungeonGenerator(Random *random, Box2 const &bounds) :
        random_(random),
        bounds_(bounds),
        minRoomSize_(5.0f),
        maxRoomSize_(7.0f),
        wallSize_(1.0f),
        corridorWidth_(2.0f),
        corridorHeight_(2.0f)
    {
        bounds_.pad(Vector2(-wallSize_, -wallSize_));
    }

    void DungeonGenerator::generate()
    {
        roomBoxes_.clear();
        corridorBoxes_.clear();
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
            float x = bounds_.p1.x + random_->getFloat() * (bounds_.getWidth() - width);
            float y = bounds_.p1.y + random_->getFloat() * (bounds_.getHeight() - height);
            Vector2 p1(x, y);
            Vector2 p2(x + width, y + height);
            Box2 box(p1, p2);
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
    
    bool DungeonGenerator::generateCorridor(Box2 const &a, Box2 const &b)
    {
        float minX = std::max(a.p1.x, b.p1.x);
        float minY = std::max(a.p1.y, b.p1.y);
        float maxX = std::min(a.p2.x, b.p2.x);
        float maxY = std::min(a.p2.y, b.p2.y);
        float width = maxX - minX;
        float height = maxY - minY;
        if (corridorWidth_ < width) {
            Box2 const &top = a.p1.y < b.p1.y ? b : a;
            Box2 const &bottom = a.p1.y < b.p1.y ? a : b;

            float topMinX = top.p1.x;
            float topMaxX = top.p2.x;
            float bottomMinX = bottom.p1.x;
            float bottomMaxX = bottom.p2.x;

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

            Vector2 p1(x, maxY);
            Vector2 p2(x + corridorWidth_, maxY - height);
            Box2 box(p1, p2);
            if (intersectsRoom(box) == 2) {
                corridorBoxes_.push_back(box);
                return true;
            }
        } else if (corridorHeight_ < height) {
            Vector2 p1(maxX, minY);
            Vector2 p2(maxX - width, minY + corridorHeight_);
            Box2 box(p1, p2);
            if (intersectsRoom(box) == 2) {
                corridorBoxes_.push_back(box);
                return true;
            }
        }
        return false;
    }

    int DungeonGenerator::intersectsRoom(Box2 const &box)
    {
        Box2 paddedBox(box);
        paddedBox.pad(Vector2(wallSize_, wallSize_));
        int count = 0;
        for (std::size_t i = 0; i < roomBoxes_.size(); ++i) {
            if (intersects(paddedBox, roomBoxes_[i])) {
                ++count;
            }
        }
        return count;
    }
}
