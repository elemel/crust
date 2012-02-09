#include "monster.hpp"

#include "debug_draw.hpp"
#include "game.hpp"
#include "geometry.hpp"
#include "monster_control_component.hpp"
#include "monster_physics_component.hpp"
#include "monster_render_component.hpp"
#include "sprite.hpp"

namespace crust {
    Monster::Monster(Game *game, Vector2 const &position) :
        game_(game),

        headDirection_(1),
        bodyDirection_(1),
    
        physicsComponent_(new MonsterPhysicsComponent(this, position)),
        controlComponent_(new MonsterControlComponent(this, physicsComponent_.get())),
        renderComponent_(new MonsterRenderComponent(this))
    { }

    Monster::~Monster()
    { }

    Vector2 Monster::getPosition() const
    {
        b2Vec2 position = physicsComponent_->getMainBody()->GetPosition();
        return Vector2(position.x, position.y);
    }

    void Monster::stepAnimation(float dt)
    {
        int xControl = int(controlComponent_->getRightControl()) - int(controlComponent_->getLeftControl());
        bool bodyTurned = false;
        if (xControl && xControl != bodyDirection_) {
            bodyDirection_ = xControl;
            renderComponent_->getBodySprite()->setScale(Vector2(0.1f * float(bodyDirection_), 0.1f));
            bodyTurned = true;
        }
        b2Vec2 localEyePosition = b2Vec2(0.0f, 0.35f);
        b2Vec2 localTargetPosition = physicsComponent_->getMainBody()->GetLocalPoint(b2Vec2(targetPosition_.x, targetPosition_.y));
        if (bodyTurned || 0.05f < std::abs(localTargetPosition.x)) {
            headDirection_ = (localTargetPosition.x < 0) ? -1 : 1;
        }
        if (bodyDirection_ == -1) {
            localTargetPosition.x = -localTargetPosition.x;
        }
        if (headDirection_ != bodyDirection_) {
            localTargetPosition.x = -localTargetPosition.x;
        }
        b2Vec2 eyeToTargetOffset = localTargetPosition - localEyePosition;
        float targetAngle = std::atan2(eyeToTargetOffset.y, eyeToTargetOffset.x);
        float headAngle = 0.5f * float(headDirection_) * targetAngle;
        renderComponent_->getHeadSprite()->setAngle(headAngle);
        renderComponent_->getHeadSprite()->setScale(Vector2(0.1f * float(headDirection_), 0.1f));
    }

    void Monster::draw() const
    {
        glPushMatrix();
        b2Vec2 const &position = physicsComponent_->getMainBody()->GetPosition();
        float angle = physicsComponent_->getMainBody()->GetAngle();
        glTranslatef(position.x, position.y, 0.0f);
        glRotatef((180.0f / M_PI) * angle, 0.0f, 0.0f, 1.0f);
        renderComponent_->draw();
        glPopMatrix();
    }
}
