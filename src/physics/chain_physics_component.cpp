#include "chain_physics_component.hpp"

#include "actor.hpp"
#include "game.hpp"

namespace crust {
    ChainPhysicsComponent::ChainPhysicsComponent(Actor *actor, Vector2 const &position, int linkCount) :
        actor_(actor),
        position_(position),
        linkCount_(linkCount),
        ropeJoint_(0)
    { }
    
    ChainPhysicsComponent::~ChainPhysicsComponent()
    { }

    void ChainPhysicsComponent::create()
    {
        b2World *world = actor_->getGame()->getPhysicsWorld();
        
        for (int i = 0; i < linkCount_; ++i) {
            b2BodyDef bodyDef;
            bodyDef.type = i ? b2_dynamicBody : b2_staticBody;
            bodyDef.position.Set(position_.x, position_.y);
            bodyDef.angle = 0.25f * M_PI;
            b2Body *body = world->CreateBody(&bodyDef);
            bodies_.push_back(body);
            b2PolygonShape shape;
            shape.SetAsBox(0.1f, 0.1f);
            b2Fixture *fixture = body->CreateFixture(&shape, 1.0f);
            b2Filter filter = fixture->GetFilterData();
            filter.groupIndex = -1;
            fixture->SetFilterData(filter);
            fixture->SetSensor(true);
            if (i) {
                body->SetLinearVelocity(b2Vec2(0.1f, 0.0f)); 
                b2RevoluteJointDef jointDef;
                b2Vec2 localAnchor = (i % 2) ? b2Vec2(-0.1f, -0.1f) : b2Vec2(0.1f, 0.1f);
                b2Vec2 anchor = body->GetWorldPoint(localAnchor);
                jointDef.Initialize(bodies_[i - 1], body, anchor);
                world->CreateJoint(&jointDef);
            }
        }
        b2RopeJointDef ropeJointDef;
        ropeJointDef.bodyA = bodies_.front();
        ropeJointDef.bodyB = bodies_.back();
        ropeJointDef.localAnchorA = b2Vec2(0.05f, 0.05f);
        ropeJointDef.localAnchorB = (bodies_.size() % 2) ? b2Vec2(0.1f, 0.1f) : b2Vec2(-0.1f, -0.1f);
        ropeJointDef.maxLength = 1.1f * 0.2f * float(bodies_.size() - 2) * M_SQRT2;
        ropeJoint_ = static_cast<b2RopeJoint *>(world->CreateJoint(&ropeJointDef));
    }

    void ChainPhysicsComponent::destroy()
    {
        b2World *world = actor_->getGame()->getPhysicsWorld();
        
        world->DestroyJoint(ropeJoint_);
        while (!bodies_.empty()) {
            world->DestroyBody(bodies_.back());
            bodies_.pop_back();
        }
    }

    Vector2 ChainPhysicsComponent::getPosition() const
    {
        b2Vec2 position = bodies_.front()->GetPosition();
        return Vector2(position.x, position.y);
    }
}
