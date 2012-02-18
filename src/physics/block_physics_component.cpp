#include "block_physics_component.hpp"

#include "actor.hpp"
#include "game.hpp"
#include "physics_service.hpp"

namespace crust {
    BlockPhysicsComponent::BlockPhysicsComponent(Actor *actor, Polygon2 const &polygon) :
        actor_(actor),
        physicsService_(actor->getGame()->getPhysicsService()),
        polygon_(polygon)
    { }

    BlockPhysicsComponent::~BlockPhysicsComponent()
    { }

    void BlockPhysicsComponent::create()
    {
        Vector2 centroid = polygon_.getCentroid();
        float angle = -M_PI + 2.0f * M_PI * actor_->getGame()->getRandomFloat();
        
        b2BodyDef bodyDef;
        bodyDef.position.Set(centroid.x, centroid.y);
        bodyDef.angle = angle;
        bodyDef.userData = actor_;
        body_ = physicsService_->getWorld()->CreateBody(&bodyDef);
        
        b2Vec2 vertices[b2_maxPolygonVertices];
        int32 vertexCount = std::min(int32(polygon_.vertices.size()),
                                     b2_maxPolygonVertices);
        
        for (int32 i = 0; i < vertexCount; ++i) {
            b2Vec2 vertex(polygon_.vertices[i].x, polygon_.vertices[i].y);
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
        
        Polygon2 innerPolygon = polygon_;
        innerPolygon.pad(-0.15f);
        for (int32 i = 0; i < vertexCount; ++i) {
            b2Vec2 vertex(innerPolygon.vertices[i].x, innerPolygon.vertices[i].y);
            vertices[i] = body_->GetLocalPoint(vertex);
        }
        b2PolygonShape innerShape;
        innerShape.Set(vertices, vertexCount);
        body_->CreateFixture(&innerShape, 0.0f);
        
        rasterize(polygon_);
    }

    void BlockPhysicsComponent::destroy()
    {
        physicsService_->getWorld()->DestroyBody(body_);
    }

    int BlockPhysicsComponent::getElement(int x, int y)
    {
        return grid_.getElement(x, y);
    }
    
    void BlockPhysicsComponent::setElement(int x, int y, int type)
    {
        grid_.setElement(x, y, type);
    }
    
    bool BlockPhysicsComponent::findElementNearPosition(float x, float y)
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
    
    int BlockPhysicsComponent::getElementAtPosition(float x, float y)
    {
        b2Vec2 localPosition = body_->GetLocalPoint(b2Vec2(x, y));
        int xIndex = int(std::floor(10.0f * localPosition.x + 0.5f));
        int yIndex = int(std::floor(10.0f * localPosition.y + 0.5f));
        return getElement(xIndex, yIndex);
    }
    
    void BlockPhysicsComponent::setElementAtPosition(float x, float y, int type)
    {
        b2Vec2 localPosition = body_->GetLocalPoint(b2Vec2(x, y));
        int xIndex = int(std::floor(10.0f * localPosition.x + 0.5f));
        int yIndex = int(std::floor(10.0f * localPosition.y + 0.5f));
        setElement(xIndex, yIndex, type);
    }
    
    Box2 BlockPhysicsComponent::getBounds() const
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
    
    bool BlockPhysicsComponent::containsPoint(Vector2 const &point) const
    {
        b2Vec2 localPoint = body_->GetLocalPoint(b2Vec2(point.x, point.y));
        return localPolygon_.containsPoint(Vector2(localPoint.x, localPoint.y));
    }
    
    void BlockPhysicsComponent::rasterize(Polygon2 const &polygon)
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
    
    void BlockPhysicsComponent::addGridPointToBounds(int x, int y, Box2 *bounds) const
    {
        b2Vec2 localPoint = b2Vec2(0.1f * float(x), 0.1f * float(y));
        b2Vec2 worldPoint = body_->GetWorldPoint(localPoint);
        bounds->mergePoint(Vector2(worldPoint.x, worldPoint.y));
    }
}
