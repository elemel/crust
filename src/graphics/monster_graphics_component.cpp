#include "monster_graphics_component.hpp"

#include "actor.hpp"
#include "convert.hpp"
#include "game.hpp"
#include "graphics_manager.hpp"
#include "monster_control_component.hpp"
#include "monster_physics_component.hpp"
#include "sprite.hpp"

#include <SDL/SDL_opengl.h>

namespace crust {
    MonsterGraphicsComponent::MonsterGraphicsComponent(Actor *actor) :
        actor_(actor),
        controlComponent_(convert(actor->getControlComponent())),
        physicsComponent_(convert(actor->getPhysicsComponent())),
        graphicsManager_(actor->getGame()->getGraphicsManager()),
    
        headDirection_(1),
        trunkDirection_(1)
    { }

    MonsterGraphicsComponent::~MonsterGraphicsComponent()
    { }

    void MonsterGraphicsComponent::create()
    {
        initSprites();
        graphicsManager_->addSprite(rightArmSprite_.get());
        graphicsManager_->addSprite(rightLegSprite_.get());
        graphicsManager_->addSprite(trunkSprite_.get());
        graphicsManager_->addSprite(headSprite_.get());
        graphicsManager_->addSprite(leftLegSprite_.get());
        graphicsManager_->addSprite(leftArmSprite_.get());
        graphicsManager_->addTask(this);
    }
    
    void MonsterGraphicsComponent::destroy()
    {
        graphicsManager_->removeTask(this);
        graphicsManager_->removeSprite(headSprite_.get());
        graphicsManager_->removeSprite(trunkSprite_.get());
        graphicsManager_->removeSprite(leftArmSprite_.get());
        graphicsManager_->removeSprite(rightArmSprite_.get());
        graphicsManager_->removeSprite(leftLegSprite_.get());
        graphicsManager_->removeSprite(rightLegSprite_.get());
    }

    void MonsterGraphicsComponent::step(float dt)
    {
        float time = actor_->getGame()->getTime();

        b2Vec2 mainBodyPosition = physicsComponent_->getMainBody()->GetPosition();
        int xControl = int(controlComponent_->getRightControl()) - int(controlComponent_->getLeftControl());
        bool anyXControl = controlComponent_->getRightControl() || controlComponent_->getLeftControl();

        bool trunkTurned = false;
        if (xControl && xControl != trunkDirection_) {
            trunkDirection_ = xControl;
            trunkSprite_->setScale(Vector2(0.1f * float(trunkDirection_), 0.1f));
            trunkTurned = true;
        }
        Vector2 trunkPosition(mainBodyPosition.x, mainBodyPosition.y - 0.15);
        trunkSprite_->setPosition(trunkPosition);

        Vector2 targetPosition = controlComponent_->getTargetPosition();
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
        float headAngle = clamp(0.5f * float(headDirection_) * targetAngle, -M_PI_4, M_PI_4);
        headSprite_->setPosition(Vector2(mainBodyPosition.x, mainBodyPosition.y + 0.25f));
        headSprite_->setAngle(headAngle);
        headSprite_->setScale(Vector2(0.1f * float(headDirection_), 0.1f));

        Vector2 leftArmPosition = trunkPosition + Vector2(-0.3f * float(trunkDirection_), 0.1f);
        float leftArmAngle = float(anyXControl) * std::sin(1.5f * 2.0f * M_PI * time + M_PI);
        leftArmSprite_->setPosition(leftArmPosition);
        leftArmSprite_->setScale(Vector2(0.1f * float(trunkDirection_), 0.1f));
        leftArmSprite_->setAngle(leftArmAngle);

        Vector2 rightArmPosition = trunkPosition + Vector2(0.3f * float(trunkDirection_), 0.1f);
        float rightArmAngle = float(anyXControl) * M_PI_4 * std::sin(1.5f * 2.0f * M_PI * time);
        rightArmSprite_->setPosition(rightArmPosition);
        rightArmSprite_->setScale(Vector2(0.1f * float(trunkDirection_), 0.1f));
        rightArmSprite_->setAngle(rightArmAngle);

        Vector2 leftLegPosition = trunkPosition + Vector2(-0.2f * float(trunkDirection_), -0.3f);
        float leftLegAngle = float(anyXControl) * M_PI_4 * std::sin(1.5f * 2.0f * M_PI * time);
        leftLegSprite_->setPosition(leftLegPosition);
        leftLegSprite_->setScale(Vector2(0.1f * float(trunkDirection_), 0.1f));
        leftLegSprite_->setAngle(leftLegAngle);
        
        Vector2 rightLegPosition = trunkPosition + Vector2(0.2f * float(trunkDirection_), -0.3f);
        float rightLegAngle = float(anyXControl) * M_PI_4 * std::sin(1.5f * 2.0f * M_PI * time + M_PI);
        rightLegSprite_->setPosition(rightLegPosition);
        rightLegSprite_->setScale(Vector2(0.1f * float(trunkDirection_), 0.1f));
        rightLegSprite_->setAngle(rightLegAngle);
    }
    
    void MonsterGraphicsComponent::initSprites()
    {
        headSprite_.reset(new Sprite);
        trunkSprite_.reset(new Sprite);
        leftArmSprite_.reset(new Sprite);
        rightArmSprite_.reset(new Sprite);
        leftLegSprite_.reset(new Sprite);
        rightLegSprite_.reset(new Sprite);

        headSprite_->setScale(Vector2(0.1f));
        trunkSprite_->setScale(Vector2(0.1f));
        leftArmSprite_->setScale(Vector2(0.1f));
        rightArmSprite_->setScale(Vector2(0.1f));
        leftLegSprite_->setScale(Vector2(0.1f));
        rightLegSprite_->setScale(Vector2(0.1f));
        
        Color4 skinColor = parseColor4("#c96");
        Color4 eyeColor = parseColor4("#000");
        Color4 noseColor = parseColor4("#c63");
        Color4 earColor = parseColor4("#c96");
        Color4 hairColor = parseColor4("#c30");
        Color4 beardColor = parseColor4("#930");
        Color4 shirtColor = parseColor4("#993");
        Color4 beltColor = parseColor4("#630");
        Color4 buckleColor = parseColor4("#fc3");
        Color4 trouserColor = parseColor4("#996");
        Color4 bootColor = parseColor4("#960");
        
        headSprite_->setPixel(-2, 2, skinColor);
        headSprite_->setPixel(-1, 2, skinColor);
        headSprite_->setPixel(0, 2, skinColor);
        headSprite_->setPixel(1, 2, skinColor);
        headSprite_->setPixel(2, 2, skinColor);
        headSprite_->setPixel(3, 2, skinColor);
        
        headSprite_->setPixel(-3, 1, skinColor);
        headSprite_->setPixel(-2, 1, skinColor);
        headSprite_->setPixel(-1, 1, skinColor);
        headSprite_->setPixel(0, 1, skinColor);
        headSprite_->setPixel(1, 1, skinColor);
        headSprite_->setPixel(2, 1, skinColor);
        headSprite_->setPixel(3, 1, skinColor);
        
        headSprite_->setPixel(-3, 0, skinColor);
        headSprite_->setPixel(-2, 0, skinColor);
        headSprite_->setPixel(-1, 0, skinColor);
        headSprite_->setPixel(0, 0, skinColor);
        headSprite_->setPixel(1, 0, skinColor);
        headSprite_->setPixel(2, 0, skinColor);
        headSprite_->setPixel(3, 0, skinColor);
        
        headSprite_->setPixel(-2, -1, skinColor);
        headSprite_->setPixel(-1, -1, skinColor);
        headSprite_->setPixel(0, -1, skinColor);
        headSprite_->setPixel(1, -1, skinColor);
        headSprite_->setPixel(2, -1, skinColor);

        headSprite_->setPixel(-2, 4, hairColor);
        headSprite_->setPixel(0, 4, hairColor);
        headSprite_->setPixel(2, 4, hairColor);
        headSprite_->setPixel(-3, 3, hairColor);
        headSprite_->setPixel(-2, 3, hairColor);
        headSprite_->setPixel(-1, 3, hairColor);
        headSprite_->setPixel(0, 3, hairColor);
        headSprite_->setPixel(1, 3, hairColor);
        headSprite_->setPixel(2, 3, hairColor);
        headSprite_->setPixel(-3, 2, hairColor);
        headSprite_->setPixel(-2, 2, hairColor);
        headSprite_->setPixel(-2, 1, hairColor);

        headSprite_->setPixel(-2, 0, beardColor);
        headSprite_->setPixel(-2, -1, beardColor);
        headSprite_->setPixel(-1, -1, beardColor);
        headSprite_->setPixel(0, -1, beardColor);
        headSprite_->setPixel(1, -1, beardColor);
        headSprite_->setPixel(2, -1, beardColor);
        headSprite_->setPixel(3, -1, beardColor);
        headSprite_->setPixel(-1, -2, beardColor);
        headSprite_->setPixel(1, -2, beardColor);
        headSprite_->setPixel(3, -2, beardColor);
        
        headSprite_->setPixel(0, 1, eyeColor);
        headSprite_->setPixel(2, 1, eyeColor);
        
        headSprite_->setPixel(1, 1, noseColor);
        headSprite_->setPixel(1, 0, noseColor);
        
        headSprite_->setPixel(-3, 1, earColor);
        headSprite_->setPixel(-3, 0, earColor);
        
        trunkSprite_->setPixel(-1, 4, skinColor);
        trunkSprite_->setPixel(0, 4, skinColor);
        trunkSprite_->setPixel(1, 4, skinColor);
        
        trunkSprite_->setPixel(-1, 3, skinColor);
        trunkSprite_->setPixel(0, 3, skinColor);
        trunkSprite_->setPixel(1, 3, skinColor);
        
        trunkSprite_->setPixel(-3, 2, shirtColor);
        trunkSprite_->setPixel(-2, 2, shirtColor);
        trunkSprite_->setPixel(-1, 2, skinColor);
        trunkSprite_->setPixel(0, 2, skinColor);
        trunkSprite_->setPixel(1, 2, skinColor);
        trunkSprite_->setPixel(2, 2, shirtColor);
        trunkSprite_->setPixel(3, 2, shirtColor);
        
        trunkSprite_->setPixel(-3, 1, shirtColor);
        trunkSprite_->setPixel(-2, 1, shirtColor);
        trunkSprite_->setPixel(-1, 1, shirtColor);
        trunkSprite_->setPixel(0, 1, shirtColor);
        trunkSprite_->setPixel(1, 1, shirtColor);
        trunkSprite_->setPixel(2, 1, shirtColor);
        trunkSprite_->setPixel(3, 1, shirtColor);
        
        trunkSprite_->setPixel(-3, 0, shirtColor);
        trunkSprite_->setPixel(-2, 0, shirtColor);
        trunkSprite_->setPixel(-1, 0, shirtColor);
        trunkSprite_->setPixel(0, 0, shirtColor);
        trunkSprite_->setPixel(1, 0, shirtColor);
        trunkSprite_->setPixel(2, 0, shirtColor);
        trunkSprite_->setPixel(3, 0, shirtColor);
        
        trunkSprite_->setPixel(-3, -1, shirtColor);
        trunkSprite_->setPixel(-2, -1, shirtColor);
        trunkSprite_->setPixel(-1, -1, shirtColor);
        trunkSprite_->setPixel(0, -1, shirtColor);
        trunkSprite_->setPixel(1, -1, shirtColor);
        trunkSprite_->setPixel(2, -1, shirtColor);
        trunkSprite_->setPixel(3, -1, shirtColor);
        
        trunkSprite_->setPixel(-3, -2, beltColor);
        trunkSprite_->setPixel(-2, -2, beltColor);
        trunkSprite_->setPixel(-1, -2, beltColor);
        trunkSprite_->setPixel(0, -2, beltColor);
        trunkSprite_->setPixel(1, -2, beltColor);
        trunkSprite_->setPixel(2, -2, beltColor);
        trunkSprite_->setPixel(3, -2, beltColor);
        
        trunkSprite_->setPixel(1, -2, buckleColor);
        trunkSprite_->setPixel(2, -2, buckleColor);
        
        trunkSprite_->setPixel(-3, -3, trouserColor);
        trunkSprite_->setPixel(-2, -3, trouserColor);
        trunkSprite_->setPixel(-1, -3, trouserColor);
        trunkSprite_->setPixel(0, -3, trouserColor);
        trunkSprite_->setPixel(1, -3, trouserColor);
        trunkSprite_->setPixel(2, -3, trouserColor);
        trunkSprite_->setPixel(3, -3, trouserColor);

        trunkSprite_->setPixel(-2, -4, trouserColor);
        trunkSprite_->setPixel(-1, -4, trouserColor);
        trunkSprite_->setPixel(0, -4, trouserColor);
        trunkSprite_->setPixel(1, -4, trouserColor);
        trunkSprite_->setPixel(2, -4, trouserColor);
        
        leftArmSprite_->setPixel(-1, 0, shirtColor);
        leftArmSprite_->setPixel(0, 0, shirtColor);
        leftArmSprite_->setPixel(-1, -1, shirtColor);
        leftArmSprite_->setPixel(0, -1, shirtColor);
        leftArmSprite_->setPixel(-1, -2, shirtColor);
        leftArmSprite_->setPixel(0, -2, shirtColor);
        leftArmSprite_->setPixel(-1, -3, skinColor);
        leftArmSprite_->setPixel(0, -3, skinColor);
        leftArmSprite_->setPixel(-1, -4, skinColor);
        leftArmSprite_->setPixel(0, -4, skinColor);
        
        rightArmSprite_->setPixel(0, 0, shirtColor);
        rightArmSprite_->setPixel(1, 0, shirtColor);
        rightArmSprite_->setPixel(0, -1, shirtColor);
        rightArmSprite_->setPixel(1, -1, shirtColor);
        rightArmSprite_->setPixel(0, -2, shirtColor);
        rightArmSprite_->setPixel(1, -2, shirtColor);
        rightArmSprite_->setPixel(0, -3, skinColor);
        rightArmSprite_->setPixel(1, -3, skinColor);
        rightArmSprite_->setPixel(0, -4, skinColor);
        rightArmSprite_->setPixel(1, -4, skinColor);

        leftLegSprite_->setPixel(-1, 0, trouserColor);
        leftLegSprite_->setPixel(0, 0, trouserColor);
        leftLegSprite_->setPixel(1, 0, trouserColor);
        leftLegSprite_->setPixel(-1, -1, trouserColor);
        leftLegSprite_->setPixel(0, -1, trouserColor);
        leftLegSprite_->setPixel(1, -1, trouserColor);
        leftLegSprite_->setPixel(-1, -2, bootColor);
        leftLegSprite_->setPixel(0, -2, bootColor);
        leftLegSprite_->setPixel(1, -2, bootColor);
        leftLegSprite_->setPixel(-1, -3, bootColor);
        leftLegSprite_->setPixel(0, -3, bootColor);
        leftLegSprite_->setPixel(1, -3, bootColor);
        leftLegSprite_->setPixel(2, -3, bootColor);
        
        rightLegSprite_->setPixel(-1, 0, trouserColor);
        rightLegSprite_->setPixel(0, 0, trouserColor);
        rightLegSprite_->setPixel(1, 0, trouserColor);
        rightLegSprite_->setPixel(-1, -1, trouserColor);
        rightLegSprite_->setPixel(0, -1, trouserColor);
        rightLegSprite_->setPixel(1, -1, trouserColor);
        rightLegSprite_->setPixel(-1, -2, bootColor);
        rightLegSprite_->setPixel(0, -2, bootColor);
        rightLegSprite_->setPixel(1, -2, bootColor);
        rightLegSprite_->setPixel(-1, -3, bootColor);
        rightLegSprite_->setPixel(0, -3, bootColor);
        rightLegSprite_->setPixel(1, -3, bootColor);
        rightLegSprite_->setPixel(2, -3, bootColor);
    }
}
