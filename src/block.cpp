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

        normalX_ = -0.05f + 0.1f * game->getRandomFloat();
        normalY_ = -0.05f + 0.1f * game->getRandomFloat();
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
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        
        if (!quadDrawVertices_.empty()) {
            glColorPointer(3, GL_FLOAT, sizeof(DrawVertex), &quadDrawVertices_[0].red);
            glNormalPointer(GL_FLOAT, sizeof(DrawVertex), &quadDrawVertices_[0].normalX);
            glVertexPointer(2, GL_FLOAT, sizeof(DrawVertex), &quadDrawVertices_[0].x);
            glDrawArrays(GL_QUADS, 0, GLsizei(quadDrawVertices_.size()));
        }
        
        if (!lineDrawVertices_.empty()) {
            glColorPointer(3, GL_FLOAT, sizeof(DrawVertex), &lineDrawVertices_[0].red);
            glNormalPointer(GL_FLOAT, sizeof(DrawVertex), &lineDrawVertices_[0].normalX);
            glVertexPointer(2, GL_FLOAT, sizeof(DrawVertex), &lineDrawVertices_[0].x);
            glDrawArrays(GL_LINES, 0, GLsizei(lineDrawVertices_.size()));
        }
        
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
                    if (box.containsPoint(worldPosition.x, worldPosition.y)) {
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

    void Block::rasterize(Polygon2 const &polygon)
    {
        Polygon2 localPolygon;
        for (int i = 0; i < polygon.getSize(); ++i) {
            b2Vec2 worldPoint(polygon.vertices[i].x, polygon.vertices[i].y);
            b2Vec2 localPoint = body_->GetLocalPoint(worldPoint);
            localPolygon.vertices.push_back(Vector2(localPoint.x, localPoint.y));
        }
        Box2 bounds = localPolygon.getBoundingBox();
        int minX = int(10.0f * bounds.p1.x + 0.05f);
        int minY = int(10.0f * bounds.p1.y + 0.05f);
        int maxX = int(10.0f * bounds.p2.x + 0.05f);
        int maxY = int(10.0f * bounds.p2.y + 0.05f);
        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                Vector2 localPoint(0.1f * float(x), 0.1f * float(y));
                if (localPolygon.containsPoint(localPoint)) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            if (dx == 0 || dy == 0) {
                                setElement(x + dx, y + dy, 1);
                            }
                        }
                    }
                }
            }
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
        bounds->mergePoint(worldPoint.x, worldPoint.y);
    }

    void Block::updateDrawVertices()
    {
        if (drawVerticesDirty_) {
            updateQuadDrawVertices();
            updateLineDrawVertices();
            drawVerticesDirty_ = false;
        }
    }

    void Block::updateQuadDrawVertices()
    {
        quadDrawVertices_.clear();
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
                    
                    vertex.normalX = normalX_ - 0.025f + 0.5f * getColorOffset(x + grid_.getX(), y + grid_.getY(), 3);
                    vertex.normalY = normalY_ - 0.025f + 0.5f * getColorOffset(x + grid_.getX(), y + grid_.getY(), 4);
                    vertex.normalZ = 1.0f;
                    
                    vertex.x = float(x + grid_.getX());
                    vertex.y = float(y + grid_.getY());
                    
                    quadDrawVertices_.push_back(vertex);
                    
                    quadDrawVertices_.push_back(vertex);
                    quadDrawVertices_.back().x += 1;
                    
                    quadDrawVertices_.push_back(vertex);
                    quadDrawVertices_.back().x += 1;
                    quadDrawVertices_.back().y += 1;
                    
                    quadDrawVertices_.push_back(vertex);
                    quadDrawVertices_.back().y += 1;
                }
            }
        }
    }

    void Block::updateLineDrawVertices()
    {
        lineDrawVertices_.clear();

        DrawVertex vertex;
        vertex.red = 0.0f;
        vertex.green = 0.0f;
        vertex.blue = 0.0f;
        vertex.normalX = 0.0f;
        vertex.normalY = 0.0f;
        vertex.normalZ = 1.0f;
        vertex.x = 0.0f;
        vertex.y = 0.0f;

        for (int y = 0; y < grid_.getHeight() + 1; ++y) {
            for (int x = 0; x < grid_.getWidth() + 1; ++x) {
                bool center = (grid_.getElement(x + grid_.getX(), y + grid_.getY())) != 0;
                bool left = (grid_.getElement(x + grid_.getX() - 1, y + grid_.getY())) != 0;
                bool bottom = (grid_.getElement(x + grid_.getX(), y + grid_.getY() - 1)) != 0;
                if (center != left) {
                    lineDrawVertices_.push_back(vertex);
                    lineDrawVertices_.back().x = float(x + grid_.getX());
                    lineDrawVertices_.back().y = float(y + grid_.getY());
                    lineDrawVertices_.push_back(vertex);
                    lineDrawVertices_.back().x = float(x + grid_.getX());
                    lineDrawVertices_.back().y = float(y + grid_.getY() + 1);
                }
                if (center != bottom) {
                    lineDrawVertices_.push_back(vertex);
                    lineDrawVertices_.back().x = float(x + grid_.getX());
                    lineDrawVertices_.back().y = float(y + grid_.getY());
                    lineDrawVertices_.push_back(vertex);
                    lineDrawVertices_.back().x = float(x + grid_.getX() + 1);
                    lineDrawVertices_.back().y = float(y + grid_.getY());
                }
            }
        }
    }
}
