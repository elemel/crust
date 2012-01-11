#include "block.hpp"

#include "game.hpp"
#include "geometry.hpp"

#include <cmath>
#include <iostream>
#include <boost/functional/hash.hpp>
#include <SDL/SDL_opengl.h>

namespace crust {
    Block::Block(Game *game, float x, float y, float angle) :
        game_(game),
        body_(0),
        red_(1.0f),
        green_(1.0f),
        blue_(1.0f),
        normalX_(0.0f),
        normalY_(0.0f),
        drawVerticesDirty_(false)
    {
        b2BodyDef bodyDef;
        bodyDef.position.Set(x, y);
        bodyDef.angle = angle;
        bodyDef.userData = this;
        body_ = game_->getPhysicsWorld()->CreateBody(&bodyDef);

        normalX_ = -0.03f + 0.06f * game->getRandomFloat();
        normalY_ = -0.03f + 0.06f * game->getRandomFloat();
    }

    Block::~Block()
    {
        game_->getPhysicsWorld()->DestroyBody(body_);
    }

    void Block::setPosition(float x, float y)
    {
        body_->SetTransform(b2Vec2(x, y), body_->GetAngle());
    }

    void Block::setAngle(float angle)
    {
        body_->SetTransform(body_->GetPosition(), angle);
    }

    void Block::setColor(float red, float green, float blue)
    {
        red_ = red;
        green_ = green;
        blue_ = blue;
    }
    
    int Block::getElement(int x, int y)
    {
        return grid_.getElement(x, y);
    }

    void Block::setElement(int x, int y, int type)
    {
        if (getElement(x, y) == 0) {
            if (getElement(x - 1, y) == 0) {
                neighbors_.push_back(std::make_pair(x - 1, y));
            }
            if (getElement(x + 1, y) == 0) {
                neighbors_.push_back(std::make_pair(x + 1, y));
            }
            if (getElement(x, y - 1) == 0) {
                neighbors_.push_back(std::make_pair(x, y - 1));
            }
            if (getElement(x, y + 1) == 0) {
                neighbors_.push_back(std::make_pair(x, y + 1));
            }
        }
        grid_.setElement(x, y, type);
        drawVerticesDirty_ = true;
    }

    bool Block::findElementNearPosition(float x, float y)
    {
        b2Vec2 localPosition = body_->GetLocalPoint(b2Vec2(x, y));
        int xIndex = int(std::floor(10.0f * localPosition.x + 0.5f));
        int yIndex = int(std::floor(10.0f * localPosition.y + 0.5f));
        return (getElement(xIndex, yIndex) ||
                getElement(xIndex - 1, yIndex) ||
                getElement(xIndex + 1, yIndex) ||
                getElement(xIndex, yIndex - 1) ||
                getElement(xIndex, yIndex + 1));
    }

    int Block::getElementAtPosition(float x, float y)
    {
        b2Vec2 localPosition = body_->GetLocalPoint(b2Vec2(x, y));
        int xIndex = int(std::floor(10.0f * localPosition.x + 0.5f));
        int yIndex = int(std::floor(10.0f * localPosition.y + 0.5f));
        return getElement(xIndex, yIndex);
    }

    void Block::setElementAtPosition(float x, float y, int type)
    {
        b2Vec2 localPosition = body_->GetLocalPoint(b2Vec2(x, y));
        int xIndex = int(std::floor(10.0f * localPosition.x + 0.5f));
        int yIndex = int(std::floor(10.0f * localPosition.y + 0.5f));
        setElement(xIndex, yIndex, type);
    }

    void Block::absorbElement(int type)
    {
        if (!neighbors_.empty()) {
            int i = game_->getRandomInt(neighbors_.size());
            int x = neighbors_[i].first;
            int y = neighbors_[i].second;
            neighbors_[i] = neighbors_.back();
            neighbors_.pop_back();
            if (findOtherBlock(x, y) == 0) {
                setElement(x, y, type);
            } else if ((getElement(x - 1, y) && findOtherBlock(x - 1, y) == 0) ||
                       (getElement(x + 1, y) && findOtherBlock(x + 1, y) == 0) ||
                       (getElement(x, y - 1) && findOtherBlock(x, y - 1) == 0) ||
                       (getElement(x, y + 1) && findOtherBlock(x, y + 1) == 0))
            {
                setElement(x, y, type);
            } else if ((getElement(x - 1, y - 1) && findOtherBlock(x - 1, y - 1) == 0) ||
                       (getElement(x + 1, y - 1) && findOtherBlock(x + 1, y - 1) == 0) ||
                       (getElement(x + 1, y + 1) && findOtherBlock(x + 1, y + 1) == 0) ||
                       (getElement(x - 1, y + 1) && findOtherBlock(x - 1, y + 1) == 0))
            {
                setElement(x, y, type);
            }
            drawVerticesDirty_ = true;
        }
    }
    
    int Block::emitElement()
    {
        return 0;
    }
    
    void Block::draw()
    {
        updateDrawVertices();

        b2Vec2 position = body_->GetPosition();
        float angle = body_->GetAngle();
        glPushAttrib(GL_CURRENT_BIT);
        glPushMatrix();
        glTranslatef(position.x, position.y, 0.0f);
        glRotatef(angle * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);
        glScalef(0.1f, 0.1f, 1.0f);
        glTranslatef(-0.5f, -0.5f, 0.0f);
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(3, GL_FLOAT, sizeof(DrawVertex), &drawVertices_[0].red);
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, sizeof(DrawVertex), &drawVertices_[0].normalX);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, sizeof(DrawVertex), &drawVertices_[0].x);
        glDrawArrays(GL_QUADS, 0, GLsizei(drawVertices_.size()));
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glPopMatrix();
        glPopAttrib();
    }

    Box2 Block::getBounds()
    {
        if (grid_.isEmpty()) {
            return Box2();
        } else {
            int x = grid_.getX();
            int y = grid_.getY();
            int width = grid_.getWidth();
            int height = grid_.getHeight();

            Box2 bounds;
            addGridPointToBounds(x, y, &bounds);
            addGridPointToBounds(x + width, y, &bounds);
            addGridPointToBounds(x + width, y + height, &bounds);
            addGridPointToBounds(x, y + height, &bounds);
            return bounds;
        }
    }

    int Block::getNeighborCount()
    {
        return int(neighbors_.size());
    }
    
    void Block::dig(Box2 const &box)
    {
        int x = grid_.getX();
        int y = grid_.getY();
        int width = grid_.getWidth();
        int height = grid_.getHeight();
        for (int dy = 0; dy < height; ++dy) {
            for (int dx = 0; dx < width; ++dx) {
                if (grid_.getElement(x + dx, y + dy)) {
                    b2Vec2 localPosition(0.1f * float(x + dx),
                                         0.1f * float(y + dy));
                    b2Vec2 worldPosition = body_->GetWorldPoint(localPosition);
                    if (box.contains(worldPosition.x, worldPosition.y)) {
                        grid_.setElement(x + dx, y + dy, 0);
                    }
                }
            }
        }
        drawVerticesDirty_ = true;
    }

    void Block::fitPhysicsShapes()
    {
        while (body_->GetFixtureList()) {
            body_->DestroyFixture(body_->GetFixtureList());
        }
        grid_.normalize();
        if (!grid_.isEmpty()) {
            float x = 0.1f * float(grid_.getX()) - 0.05f;
            float y = 0.1f * float(grid_.getY()) - 0.05f;
            float width = 0.1f * float(grid_.getWidth());
            float height = 0.1f * float(grid_.getHeight());

            b2Vec2 center(x + 0.5f * width, y + 0.5f * height);
            b2PolygonShape shape;            
            shape.SetAsBox(0.5f * width, 0.5f * height, center, 0.0f);

            body_->CreateFixture(&shape, 1.0f);
        }
    }

    float Block::getColorOffset(int x, int y, int i)
    {
        std::size_t a = hashValue((x << 16) + (y << 8) + i);
        return 0.1f * 0.001f * float(a % 1000);
    }
    
    // http://burtleburtle.net/bob/hash/integer.html
    std::size_t Block::hashValue(std::size_t a)
    {
        a -= (a << 6);
        a ^= (a >> 17);
        a -= (a << 9);
        a ^= (a << 4);
        a -= (a << 3);
        a ^= (a << 10);
        a ^= (a >> 15);
        return a;
    }

    Block *Block::findOtherBlock(int x, int y)
    {
        b2Vec2 localPosition(0.1f * float(x), 0.1f * float(y));
        b2Vec2 worldPosition = body_->GetWorldPoint(localPosition);
        return game_->findBlockAtPosition(worldPosition.x, worldPosition.y, this);
    }

    void Block::addGridPointToBounds(int x, int y, Box2 *bounds)
    {
        b2Vec2 localPoint = b2Vec2(0.1f * float(x), 0.1f * float(y));
        b2Vec2 worldPoint = body_->GetWorldPoint(localPoint);
        bounds->merge(worldPoint.x, worldPoint.y);
    }

    void Block::updateDrawVertices()
    {
        if (drawVerticesDirty_) {
            drawVertices_.clear();
            for (int y = 0; y < grid_.getHeight(); ++y) {
                for (int x = 0; x < grid_.getWidth(); ++x) {
                    int type = grid_.getElement(x + grid_.getX(), y + grid_.getY());
                    if (type) {
                        DrawVertex vertex;
                        vertex.red = red_ + getColorOffset(x + grid_.getX(), y + grid_.getY(), 0);
                        vertex.green = green_ + getColorOffset(x + grid_.getX(), y + grid_.getY(), 1);
                        vertex.blue = blue_ + getColorOffset(x + grid_.getX(), y + grid_.getY(), 2);
                        if (!neighbors_.empty()) {
                            std::swap(vertex.red, vertex.blue);
                        }

                        float normalX = 0.0f;
                        float normalY = 0.0f;
                        getNormal(x + grid_.getX(), y + grid_.getY(), &normalX, &normalY);
                        vertex.normalX = normalX_ + normalX;
                        vertex.normalY = normalY_ + normalY;
                        vertex.normalZ = 1.0f;
                        
                        vertex.x = float(x + grid_.getX());
                        vertex.y = float(y + grid_.getY());
                        
                        drawVertices_.push_back(vertex);
                        
                        drawVertices_.push_back(vertex);
                        drawVertices_.back().x += 1;
                        
                        drawVertices_.push_back(vertex);
                        drawVertices_.back().x += 1;
                        drawVertices_.back().y += 1;
                        
                        drawVertices_.push_back(vertex);
                        drawVertices_.back().y += 1;
                    }
                }
            }
            drawVerticesDirty_ = false;
        }
    }

    void Block::getNormal(int x, int y, float *normalX, float *normalY)
    {
        bool north = grid_.getElement(x + 0, y + 1);
        bool west = grid_.getElement(x - 1, y + 0);
        bool south = grid_.getElement(x + 0, y - 1);
        bool east = grid_.getElement(x + 1, y + 0);

        if (!north || !west || !south || !east) {
            *normalX = -0.03f * (!east ? -1.0f : !west ? 1.0f : 0.0f);
            *normalY = -0.03f * (!north ? -1.0f : !south ? 1.0f : 0.0f);
            return;
        }

        bool northwest = grid_.getElement(x - 1, y + 1);
        bool southwest = grid_.getElement(x - 1, y - 1);
        bool southeast = grid_.getElement(x + 1, y - 1);
        bool northeast = grid_.getElement(x + 1, y + 1);

        if (!northwest || !southwest || !southeast || !northeast) {
            *normalX = -0.02f * float(int(southeast) + int(northeast) - int(northwest) - int(southwest));
            *normalY = -0.02f * float(int(northwest) + int(northeast) - int(southwest) - int(southeast));
            return;
        }
        
        bool farNorth = grid_.getElement(x + 0, y + 2) && grid_.getElement(x - 1, y + 2) && grid_.getElement(x + 1, y + 2);
        bool farWest = grid_.getElement(x - 2, y + 0) && grid_.getElement(x - 2, y - 1) && grid_.getElement(x - 2, y + 1);
        bool farSouth = grid_.getElement(x + 0, y - 2) && grid_.getElement(x - 1, y - 2) && grid_.getElement(x + 1, y - 2);
        bool farEast = grid_.getElement(x + 2, y + 0) && grid_.getElement(x + 2, y - 1) && grid_.getElement(x + 2, y + 1);
        
        *normalX = -0.01f * float(int(farEast) - int(farWest));
        *normalY = -0.01f * float(int(farNorth) - int(farSouth));
    }
}
