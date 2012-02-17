#include "monster_animation_component.hpp"

#include "actor.hpp"
#include "animation_service.hpp"
#include "convert.hpp"
#include "game.hpp"
#include "monster_control_component.hpp"
#include "monster_physics_component.hpp"
#include "monster_scene_component.hpp"
#include "sprite.hpp"

namespace crust {
    MonsterAnimationComponent::MonsterAnimationComponent(Actor *actor) :
        actor_(actor),
        physicsComponent_(convert(actor->getPhysicsComponent())),
        controlComponent_(convert(actor->getControlComponent())),
        sceneComponent_(convert(actor->getSceneComponent())),
        animationService_(actor->getGame()->getAnimationService()),

        headDirection_(1),
        trunkDirection_(1)
    { }

    void MonsterAnimationComponent::create()
    {
        animationService_->addTask(this);
    }
    
    void MonsterAnimationComponent::destroy()
    {
        animationService_->removeTask(this);
    }

    void MonsterAnimationComponent::step(float dt)
    {
        b2Vec2 mainBodyPosition = physicsComponent_->getMainBody()->GetPosition();
        int xControl = int(controlComponent_->getRightControl()) - int(controlComponent_->getLeftControl());
        bool trunkTurned = false;
        if (xControl && xControl != trunkDirection_) {
            trunkDirection_ = xControl;
            sceneComponent_->getTrunkSprite()->setScale(Vector2(0.1f * float(trunkDirection_), 0.1f));
            trunkTurned = true;
        }
        sceneComponent_->getTrunkSprite()->setPosition(Vector2(mainBodyPosition.x, mainBodyPosition.y - 0.15f));
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
        sceneComponent_->getHeadSprite()->setPosition(Vector2(mainBodyPosition.x, mainBodyPosition.y + 0.25f));
        sceneComponent_->getHeadSprite()->setAngle(headAngle);
        sceneComponent_->getHeadSprite()->setScale(Vector2(0.1f * float(headDirection_), 0.1f));
    }
}
