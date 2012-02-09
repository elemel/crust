#include "block.hpp"

#include "game.hpp"
#include "geometry.hpp"
#include "block_render_component.hpp"

#include <cmath>
#include <iostream>
#include <boost/functional/hash.hpp>
#include <SDL/SDL_opengl.h>

namespace crust {
    Block::Block(Game *game, Polygon2 const &polygon) :
        Actor(game),
        body_(0)
    {
        Vector2 centroid = polygon.getCentroid();
        float angle = -M_PI + 2.0f * M_PI * game->getRandomFloat();

        b2BodyDef bodyDef;
        bodyDef.position.Set(centroid.x, centroid.y);
        bodyDef.angle = angle;
        bodyDef.userData = static_cast<Actor *>(this);
        body_ = game_->getPhysicsWorld()->CreateBody(&bodyDef);

        b2Vec2 vertices[b2_maxPolygonVertices];
        int32 vertexCount = std::min(int32(polygon.vertices.size()),
                                     b2_maxPolygonVertices);

        for (int32 i = 0; i < vertexCount; ++i) {
            b2Vec2 vertex(polygon.vertices[i].x, polygon.vertices[i].y);
            vertices[i] = body_->GetLocalPoint(vertex);
            localPolygon_.vertices.push_back(Vector2(vertices[i].x, vertices[i].y));
        }
        b2PolygonShape shape;
        shape.Set(vertices, vertexCount);
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = 2.5f;
        fixtureDef.filter.groupIndex = -1;
        body_->CreateFixture(&fixtureDef);

        Polygon2 innerPolygon = polygon;
        innerPolygon.pad(-0.15f);
        for (int32 i = 0; i < vertexCount; ++i) {
            b2Vec2 vertex(innerPolygon.vertices[i].x, innerPolygon.vertices[i].y);
            vertices[i] = body_->GetLocalPoint(vertex);
        }
        b2PolygonShape innerShape;
        innerShape.Set(vertices, vertexCount);
        body_->CreateFixture(&innerShape, 0.0f);

        rasterize(polygon);

        renderComponent_.reset(new BlockRenderComponent(this));
    }

    Block::~Block()
    {
        game_->getPhysicsWorld()->DestroyBody(body_);
    }

    Vector2 Block::getPosition() const
    {
        b2Vec2 position = body_->GetPosition();
        return Vector2(position.x, position.y);
    }

    void Block::setPosition(float x, float y)
    {
        body_->SetTransform(b2Vec2(x, y), body_->GetAngle());
    }

    void Block::setAngle(float angle)
    {
        body_->SetTransform(body_->GetPosition(), angle);
    }
    
    int Block::getElement(int x, int y)
    {
        return grid_.getElement(x, y);
    }

    void Block::setElement(int x, int y, int type)
    {
        grid_.setElement(x, y, type);
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

    Box2 Block::getBounds() const
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

    bool Block::containsPoint(Vector2 const &point) const
    {
        b2Vec2 localPoint = body_->GetLocalPoint(b2Vec2(point.x, point.y));
        return localPolygon_.containsPoint(Vector2(localPoint.x, localPoint.y));
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
    
    void Block::addGridPointToBounds(int x, int y, Box2 *bounds) const
    {
        b2Vec2 localPoint = b2Vec2(0.1f * float(x), 0.1f * float(y));
        b2Vec2 worldPoint = body_->GetWorldPoint(localPoint);
        bounds->mergePoint(worldPoint.x, worldPoint.y);
    }
}
