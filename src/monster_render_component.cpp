#include "monster_render_component.hpp"

#include "monster.hpp"
#include "monster_physics_component.hpp"
#include "sprite.hpp"
#include "wire.hpp"

#include <SDL/SDL_opengl.h>

namespace crust {
    MonsterRenderComponent::MonsterRenderComponent(Monster *monster) :
        monster_(monster),
        physicsComponent_(wire(monster->getPhysicsComponent()))
    {
        initSprites();
    }

    void MonsterRenderComponent::draw() const
    {
        glPushMatrix();
        Vector2 const &position = physicsComponent_->getPosition();
        float angle = physicsComponent_->getAngle();
        glTranslatef(position.x, position.y, 0.0f);
        glRotatef((180.0f / M_PI) * angle, 0.0f, 0.0f, 1.0f);
        trunkSprite_->draw();
        headSprite_->draw();
        glPopMatrix();
    }
    
    void MonsterRenderComponent::initSprites()
    {
        headSprite_.reset(new Sprite);
        trunkSprite_.reset(new Sprite);

        headSprite_->setScale(Vector2(0.1f));
        trunkSprite_->setScale(Vector2(0.1f));
        
        Color4 skinColor = parseColor4("#fc9");
        Color4 eyeColor = parseColor4("#000");
        Color4 noseColor = parseColor4("#f96");
        Color4 earColor = parseColor4("#f96");
        Color4 hairColor = parseColor4("#996");
        Color4 beardColor = parseColor4("#663");
        Color4 shirtColor = parseColor4("#6cf");
        Color4 beltColor = parseColor4("#630");
        Color4 buckleColor = parseColor4("#fc3");
        Color4 trouserColor = parseColor4("#069");
        Color4 bootColor = parseColor4("#960");
        
        headSprite_->setPixel(Vector2(-0.2f, 0.2f), skinColor);
        headSprite_->setPixel(Vector2(-0.1f, 0.2f), skinColor);
        headSprite_->setPixel(Vector2(0.0f, 0.2f), skinColor);
        headSprite_->setPixel(Vector2(0.1f, 0.2f), skinColor);
        headSprite_->setPixel(Vector2(0.2f, 0.2f), skinColor);
        headSprite_->setPixel(Vector2(0.3f, 0.2f), skinColor);
        
        headSprite_->setPixel(Vector2(-0.3f, 0.1f), skinColor);
        headSprite_->setPixel(Vector2(-0.2f, 0.1f), skinColor);
        headSprite_->setPixel(Vector2(-0.1f, 0.1f), skinColor);
        headSprite_->setPixel(Vector2(0.0f, 0.1f), skinColor);
        headSprite_->setPixel(Vector2(0.1f, 0.1f), skinColor);
        headSprite_->setPixel(Vector2(0.2f, 0.1f), skinColor);
        headSprite_->setPixel(Vector2(0.3f, 0.1f), skinColor);
        
        headSprite_->setPixel(Vector2(-0.3f, 0.0f), skinColor);
        headSprite_->setPixel(Vector2(-0.2f, 0.0f), skinColor);
        headSprite_->setPixel(Vector2(-0.1f, 0.0f), skinColor);
        headSprite_->setPixel(Vector2(0.0f, 0.0f), skinColor);
        headSprite_->setPixel(Vector2(0.1f, 0.0f), skinColor);
        headSprite_->setPixel(Vector2(0.2f, 0.0f), skinColor);
        headSprite_->setPixel(Vector2(0.3f, 0.0f), skinColor);
        
        headSprite_->setPixel(Vector2(-0.2f, -0.1f), skinColor);
        headSprite_->setPixel(Vector2(-0.1f, -0.1f), skinColor);
        headSprite_->setPixel(Vector2(0.0f, -0.1f), skinColor);
        headSprite_->setPixel(Vector2(0.1f, -0.1f), skinColor);
        headSprite_->setPixel(Vector2(0.2f, -0.1f), skinColor);
        
        headSprite_->setPixel(Vector2(-0.2f, 0.4f), hairColor);
        headSprite_->setPixel(Vector2(0.0f, 0.4f), hairColor);
        headSprite_->setPixel(Vector2(0.2f, 0.4f), hairColor);
        headSprite_->setPixel(Vector2(-0.3f, 0.3f), hairColor);
        headSprite_->setPixel(Vector2(-0.2f, 0.3f), hairColor);
        headSprite_->setPixel(Vector2(-0.1f, 0.3f), hairColor);
        headSprite_->setPixel(Vector2(0.0f, 0.3f), hairColor);
        headSprite_->setPixel(Vector2(0.1f, 0.3f), hairColor);
        headSprite_->setPixel(Vector2(0.2f, 0.3f), hairColor);
        headSprite_->setPixel(Vector2(-0.3f, 0.2f), hairColor);
        headSprite_->setPixel(Vector2(-0.2f, 0.2f), hairColor);
        headSprite_->setPixel(Vector2(-0.2f, 0.1f), hairColor);
        
        headSprite_->setPixel(Vector2(-0.2f, 0.0f), beardColor);
        headSprite_->setPixel(Vector2(-0.2f, -0.1f), beardColor);
        headSprite_->setPixel(Vector2(-0.1f, -0.1f), beardColor);
        headSprite_->setPixel(Vector2(0.0f, -0.1f), beardColor);
        headSprite_->setPixel(Vector2(0.1f, -0.1f), beardColor);
        headSprite_->setPixel(Vector2(0.2f, -0.1f), beardColor);
        headSprite_->setPixel(Vector2(0.3f, -0.1f), beardColor);
        headSprite_->setPixel(Vector2(-0.1f, -0.2f), beardColor);
        headSprite_->setPixel(Vector2(0.1f, -0.2f), beardColor);
        headSprite_->setPixel(Vector2(0.3f, -0.2f), beardColor);
        
        headSprite_->setPixel(Vector2(0.0f, 0.1f), eyeColor);
        headSprite_->setPixel(Vector2(0.2f, 0.1f), eyeColor);
        
        headSprite_->setPixel(Vector2(0.1f, 0.1f), noseColor);
        headSprite_->setPixel(Vector2(0.1f, 0.0f), noseColor);
        
        headSprite_->setPixel(Vector2(-0.3f, 0.1f), earColor);
        headSprite_->setPixel(Vector2(-0.3f, 0.0f), earColor);
        
        trunkSprite_->setPixel(Vector2(-0.1f, 0.4f), skinColor);
        trunkSprite_->setPixel(Vector2(0.0f, 0.4f), skinColor);
        trunkSprite_->setPixel(Vector2(0.1f, 0.4f), skinColor);
        
        trunkSprite_->setPixel(Vector2(-0.1f, 0.3f), skinColor);
        trunkSprite_->setPixel(Vector2(0.0f, 0.3f), skinColor);
        trunkSprite_->setPixel(Vector2(0.1f, 0.3f), skinColor);
        
        trunkSprite_->setPixel(Vector2(-0.4f, 0.2f), shirtColor);
        trunkSprite_->setPixel(Vector2(-0.3f, 0.2f), shirtColor);
        trunkSprite_->setPixel(Vector2(-0.2f, 0.2f), shirtColor);
        trunkSprite_->setPixel(Vector2(-0.1f, 0.2f), skinColor);
        trunkSprite_->setPixel(Vector2(0.0f, 0.2f), skinColor);
        trunkSprite_->setPixel(Vector2(0.1f, 0.2f), skinColor);
        trunkSprite_->setPixel(Vector2(0.2f, 0.2f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.3f, 0.2f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.4f, 0.2f), shirtColor);
        
        trunkSprite_->setPixel(Vector2(-0.5f, 0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(-0.4f, 0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(-0.3f, 0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(-0.2f, 0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(-0.1f, 0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.0f, 0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.1f, 0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.2f, 0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.3f, 0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.4f, 0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.5f, 0.1f), shirtColor);
        
        trunkSprite_->setPixel(Vector2(-0.5f, 0.0f), shirtColor);
        trunkSprite_->setPixel(Vector2(-0.3f, 0.0f), shirtColor);
        trunkSprite_->setPixel(Vector2(-0.2f, 0.0f), shirtColor);
        trunkSprite_->setPixel(Vector2(-0.1f, 0.0f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.0f, 0.0f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.1f, 0.0f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.2f, 0.0f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.3f, 0.0f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.5f, 0.0f), shirtColor);
        
        trunkSprite_->setPixel(Vector2(-0.5f, -0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(-0.3f, -0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(-0.2f, -0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(-0.1f, -0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.0f, -0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.1f, -0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.2f, -0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.3f, -0.1f), shirtColor);
        trunkSprite_->setPixel(Vector2(0.5f, -0.1f), shirtColor);
        
        trunkSprite_->setPixel(Vector2(-0.3f, -0.2f), beltColor);
        trunkSprite_->setPixel(Vector2(-0.2f, -0.2f), beltColor);
        trunkSprite_->setPixel(Vector2(-0.1f, -0.2f), beltColor);
        trunkSprite_->setPixel(Vector2(0.0f, -0.2f), beltColor);
        trunkSprite_->setPixel(Vector2(0.1f, -0.2f), beltColor);
        trunkSprite_->setPixel(Vector2(0.2f, -0.2f), beltColor);
        trunkSprite_->setPixel(Vector2(0.3f, -0.2f), beltColor);
        
        trunkSprite_->setPixel(Vector2(0.0f, -0.2f), buckleColor);
        trunkSprite_->setPixel(Vector2(0.1f, -0.2f), buckleColor);
        
        trunkSprite_->setPixel(Vector2(-0.5f, -0.2f), skinColor);
        trunkSprite_->setPixel(Vector2(-0.5f, -0.3f), skinColor);
        
        trunkSprite_->setPixel(Vector2(0.5f, -0.2f), skinColor);
        trunkSprite_->setPixel(Vector2(0.5f, -0.3f), skinColor);
        
        trunkSprite_->setPixel(Vector2(-0.3f, -0.3f), trouserColor);
        trunkSprite_->setPixel(Vector2(-0.2f, -0.3f), trouserColor);
        trunkSprite_->setPixel(Vector2(-0.1f, -0.3f), trouserColor);
        trunkSprite_->setPixel(Vector2(0.0f, -0.3f), trouserColor);
        trunkSprite_->setPixel(Vector2(0.1f, -0.3f), trouserColor);
        trunkSprite_->setPixel(Vector2(0.2f, -0.3f), trouserColor);
        trunkSprite_->setPixel(Vector2(0.3f, -0.3f), trouserColor);
        
        trunkSprite_->setPixel(Vector2(-0.3f, -0.4f), trouserColor);
        trunkSprite_->setPixel(Vector2(-0.2f, -0.4f), trouserColor);
        trunkSprite_->setPixel(Vector2(0.2f, -0.4f), trouserColor);
        trunkSprite_->setPixel(Vector2(0.3f, -0.4f), trouserColor);
        
        trunkSprite_->setPixel(Vector2(-0.3f, -0.5f), bootColor);
        trunkSprite_->setPixel(Vector2(-0.2f, -0.5f), bootColor);
        trunkSprite_->setPixel(Vector2(0.2f, -0.5f), bootColor);
        trunkSprite_->setPixel(Vector2(0.3f, -0.5f), bootColor);
        
        trunkSprite_->setPixel(Vector2(-0.3f, -0.6f), bootColor);
        trunkSprite_->setPixel(Vector2(-0.2f, -0.6f), bootColor);
        trunkSprite_->setPixel(Vector2(-0.1f, -0.6f), bootColor);
        trunkSprite_->setPixel(Vector2(0.2f, -0.6f), bootColor);
        trunkSprite_->setPixel(Vector2(0.3f, -0.6f), bootColor);
        trunkSprite_->setPixel(Vector2(0.4f, -0.6f), bootColor);
        
        headSprite_->setPosition(Vector2(0.0f, 0.25f));
        trunkSprite_->setPosition(Vector2(0.0f, -0.15f));
    }
}
