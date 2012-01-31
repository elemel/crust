#include "chain.hpp"

#include "game.hpp"

namespace crust {
    Chain::Chain(Game *game, Vector2 const &position, int linkCount) :
        game_(game),
        ropeJoint_(0)
    {
        for (int i = 0; i < linkCount; ++i) {
            b2BodyDef bodyDef;
            bodyDef.type = i ? b2_dynamicBody : b2_staticBody;
            bodyDef.position.Set(position.x, position.y);
            bodyDef.angle = 0.25f * M_PI;
            b2Body *body = game_->getPhysicsWorld()->CreateBody(&bodyDef);
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
                game_->getPhysicsWorld()->CreateJoint(&jointDef);
            }
        }
        b2RopeJointDef ropeJointDef;
        ropeJointDef.bodyA = bodies_.front();
        ropeJointDef.bodyB = bodies_.back();
        ropeJointDef.localAnchorA = b2Vec2(0.05f, 0.05f);
        ropeJointDef.localAnchorB = (bodies_.size() % 2) ? b2Vec2(0.1f, 0.1f) : b2Vec2(-0.1f, -0.1f);
        ropeJointDef.maxLength = 1.1f * 0.2f * float(bodies_.size() - 2) * M_SQRT2;
        ropeJoint_ = static_cast<b2RopeJoint *>(game_->getPhysicsWorld()->CreateJoint(&ropeJointDef));
    }

    Chain::~Chain()
    {
        game_->getPhysicsWorld()->DestroyJoint(ropeJoint_);
        while (!bodies_.empty()) {
            game_->getPhysicsWorld()->DestroyBody(bodies_.back());
            bodies_.pop_back();
        }
    }

    Vector2 Chain::getPosition() const
    {
        b2Vec2 position = bodies_.front()->GetPosition();
        return Vector2(position.x, position.y);
    }

    void Chain::draw() const
    {
        for (std::size_t i = 0; i < bodies_.size(); ++i) {
            b2Vec2 position = bodies_[i]->GetPosition();
            float angle = bodies_[i]->GetAngle();
            glPushMatrix();
            glTranslatef(position.x, position.y, 0.0f);
            glRotatef(180.0f / M_PI * angle, 0.0f, 0.0f, 1.0f);
            glColor3ub(255, 255, 255);
            glBegin(GL_QUADS);
            glVertex2f(-0.1f, -0.1f);
            glVertex2f(0.0f, -0.1f);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(-0.1f, 0.0f);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(0.1f, 0.0f);
            glVertex2f(0.1f, 0.1f);
            glVertex2f(0.0f, 0.1f);
            glEnd();
            glColor3ub(0, 0, 0);
            glBegin(GL_LINES);
            glVertex2f(-0.1f, -0.1f);
            glVertex2f(0.0f, -0.1f);
            glVertex2f(0.0f, -0.1f);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(-0.1f, 0.0f);
            glVertex2f(-0.1f, 0.0f);
            glVertex2f(-0.1f, -0.1f);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(0.1f, 0.0f);
            glVertex2f(0.1f, 0.0f);
            glVertex2f(0.1f, 0.1f);
            glVertex2f(0.1f, 0.1f);
            glVertex2f(0.0f, 0.1f);
            glVertex2f(0.0f, 0.1f);
            glVertex2f(0.0f, 0.0f);
            glEnd();
            glPopMatrix();
        }
    }
}