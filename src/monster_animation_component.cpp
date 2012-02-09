#include "monster_animation_component.hpp"

#include "monster.hpp"
#include "monster_control_component.hpp"
#include "monster_physics_component.hpp"
#include "monster_render_component.hpp"
#include "sprite.hpp"

namespace crust {
    MonsterAnimationComponent::MonsterAnimationComponent(Monster *monster,
                                                         MonsterPhysicsComponent *physicsComponent,
                                                         MonsterControlComponent *controlComponent,
                                                         MonsterRenderComponent *renderComponent) :
        monster_(monster),
        physicsComponent_(physicsComponent),
        controlComponent_(controlComponent),
        renderComponent_(renderComponent),
    
        headDirection_(1),
        trunkDirection_(1)
    { }

    void MonsterAnimationComponent::step(float dt)
    {
        int xControl = int(controlComponent_->getRightControl()) - int(controlComponent_->getLeftControl());
        bool trunkTurned = false;
        if (xControl && xControl != trunkDirection_) {
            trunkDirection_ = xControl;
            renderComponent_->getTrunkSprite()->setScale(Vector2(0.1f * float(trunkDirection_), 0.1f));
            trunkTurned = true;
        }
        Vector2 const &targetPosition = controlComponent_->getTargetPosition();
        b2Vec2 localEyePosition = b2Vec2(0.0f, 0.35f);
        b2Vec2 localTargetPosition = physicsComponent_->getMainBody()->GetLocalPoint(b2Vec2(targetPosition.x, targetPosition.y));
        if (trunkTurned || 0.05f < std::abs(localTargetPosition.x)) {
            headDirection_ = (localTargetPosition.x < 0) ? -1 : 1;
        }
        if (trunkDirection_ == -1) {
            localTargetPosition.x = -localTargetPosition.x;
        }
        if (headDirection_ != trunkDirection_) {
            localTargetPosition.x = -localTargetPosition.x;
        }
        b2Vec2 eyeToTargetOffset = localTargetPosition - localEyePosition;
        float targetAngle = std::atan2(eyeToTargetOffset.y, eyeToTargetOffset.x);
        float headAngle = 0.5f * float(headDirection_) * targetAngle;
        renderComponent_->getHeadSprite()->setAngle(headAngle);
        renderComponent_->getHeadSprite()->setScale(Vector2(0.1f * float(headDirection_), 0.1f));
    }
}
