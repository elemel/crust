#include "particle_emitter.hpp"

#include "game.hpp"

namespace crust {
    ParticleEmitter::ParticleEmitter(Game *game) :
        game_(game),
        x_(0.0f),
        y_(0.0f),
        enabled_(false),
        targetX_(0.0f),
        targetY_(0.0f),
        nextEmitTime_(0.0f),
        emitInterval_(0.01f),
        minTargetDistance_(0.1f),
        particleVelocity_(10.0f)
    { }

    void ParticleEmitter::setPosition(float x, float y)
    {
        x_ = x;
        y_ = y;
    }

    void ParticleEmitter::setEnabled(bool enabled)
    {
        enabled_ = enabled;
    }
    
    void ParticleEmitter::setTargetPosition(float x, float y)
    {
        targetX_ = x;
        targetY_ = y;
    }

    void ParticleEmitter::step(float dt)
    {
        while (nextEmitTime_ < game_->getTime()) {
            nextEmitTime_ += emitInterval_;
            emit();
        }
    }

    void ParticleEmitter::draw()
    {
        drawPosition();
        drawTargetPosition();
    }

    void ParticleEmitter::drawPosition()
    {
        float halfSize = 0.05f;
        glBegin(GL_LINE_LOOP);
        glVertex2f(x_ - halfSize, y_ - halfSize);
        glVertex2f(x_ + halfSize, y_ - halfSize);
        glVertex2f(x_ + halfSize, y_ + halfSize);
        glVertex2f(x_ - halfSize, y_ + halfSize);
        glEnd();
    }

    void ParticleEmitter::drawTargetPosition()
    {
        float halfSize = 0.05f;
        glBegin(GL_LINE_LOOP);
        glVertex2f(targetX_ - halfSize, targetY_ - halfSize);
        glVertex2f(targetX_ + halfSize, targetY_ - halfSize);
        glVertex2f(targetX_ + halfSize, targetY_ + halfSize);
        glVertex2f(targetX_ - halfSize, targetY_ + halfSize);
        glEnd();
    }

    void ParticleEmitter::emit()
    {
        b2Vec2 targetOffset = b2Vec2(targetX_, targetY_) - b2Vec2(x_, y_);
        if (enabled_ && targetOffset.Length() > minTargetDistance_) {
            b2Vec2 velocity = targetOffset;
            velocity.Normalize();
            velocity *= particleVelocity_;

            b2BodyDef bodyDef;
            bodyDef.type = b2_dynamicBody;
            bodyDef.linearVelocity = velocity;
            bodyDef.gravityScale = 0.0f;
            b2Body *body = game_->getPhysicsWorld()->CreateBody(&bodyDef);
            
            b2CircleShape shape;
            shape.m_radius = 0.05f;
            
            b2FixtureDef fixtureDef;
            fixtureDef.shape = &shape;
            fixtureDef.isSensor = true;
            
            body->CreateFixture(&fixtureDef);
        }
    }
}
