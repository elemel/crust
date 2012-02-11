#include "monster_render_component.hpp"

#include "actor.hpp"
#include "monster_physics_component.hpp"
#include "sprite.hpp"
#include "wire.hpp"

#include <SDL/SDL_opengl.h>

namespace crust {
    MonsterRenderComponent::MonsterRenderComponent(Actor *actor) :
        actor_(actor),
        physicsComponent_(wire(actor->getPhysicsComponent()))
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
        
        trunkSprite_->setPixel(-4, 2, shirtColor);
        trunkSprite_->setPixel(-3, 2, shirtColor);
        trunkSprite_->setPixel(-2, 2, shirtColor);
        trunkSprite_->setPixel(-1, 2, skinColor);
        trunkSprite_->setPixel(0, 2, skinColor);
        trunkSprite_->setPixel(1, 2, skinColor);
        trunkSprite_->setPixel(2, 2, shirtColor);
        trunkSprite_->setPixel(3, 2, shirtColor);
        trunkSprite_->setPixel(4, 2, shirtColor);
        
        trunkSprite_->setPixel(-5, 1, shirtColor);
        trunkSprite_->setPixel(-4, 1, shirtColor);
        trunkSprite_->setPixel(-3, 1, shirtColor);
        trunkSprite_->setPixel(-2, 1, shirtColor);
        trunkSprite_->setPixel(-1, 1, shirtColor);
        trunkSprite_->setPixel(0, 1, shirtColor);
        trunkSprite_->setPixel(1, 1, shirtColor);
        trunkSprite_->setPixel(2, 1, shirtColor);
        trunkSprite_->setPixel(3, 1, shirtColor);
        trunkSprite_->setPixel(4, 1, shirtColor);
        trunkSprite_->setPixel(5, 1, shirtColor);
        
        trunkSprite_->setPixel(-5, 0, shirtColor);
        trunkSprite_->setPixel(-3, 0, shirtColor);
        trunkSprite_->setPixel(-2, 0, shirtColor);
        trunkSprite_->setPixel(-1, 0, shirtColor);
        trunkSprite_->setPixel(0, 0, shirtColor);
        trunkSprite_->setPixel(1, 0, shirtColor);
        trunkSprite_->setPixel(2, 0, shirtColor);
        trunkSprite_->setPixel(3, 0, shirtColor);
        trunkSprite_->setPixel(5, 0, shirtColor);
        
        trunkSprite_->setPixel(-5, -1, shirtColor);
        trunkSprite_->setPixel(-3, -1, shirtColor);
        trunkSprite_->setPixel(-2, -1, shirtColor);
        trunkSprite_->setPixel(-1, -1, shirtColor);
        trunkSprite_->setPixel(0, -1, shirtColor);
        trunkSprite_->setPixel(1, -1, shirtColor);
        trunkSprite_->setPixel(2, -1, shirtColor);
        trunkSprite_->setPixel(3, -1, shirtColor);
        trunkSprite_->setPixel(5, -1, shirtColor);
        
        trunkSprite_->setPixel(-3, -2, beltColor);
        trunkSprite_->setPixel(-2, -2, beltColor);
        trunkSprite_->setPixel(-1, -2, beltColor);
        trunkSprite_->setPixel(0, -2, beltColor);
        trunkSprite_->setPixel(1, -2, beltColor);
        trunkSprite_->setPixel(2, -2, beltColor);
        trunkSprite_->setPixel(3, -2, beltColor);
        
        trunkSprite_->setPixel(0, -2, buckleColor);
        trunkSprite_->setPixel(1, -2, buckleColor);
        
        trunkSprite_->setPixel(-5, -2, skinColor);
        trunkSprite_->setPixel(-5, -3, skinColor);
        
        trunkSprite_->setPixel(5, -2, skinColor);
        trunkSprite_->setPixel(5, -3, skinColor);
        
        trunkSprite_->setPixel(-3, -3, trouserColor);
        trunkSprite_->setPixel(-2, -3, trouserColor);
        trunkSprite_->setPixel(-1, -3, trouserColor);
        trunkSprite_->setPixel(0, -3, trouserColor);
        trunkSprite_->setPixel(1, -3, trouserColor);
        trunkSprite_->setPixel(2, -3, trouserColor);
        trunkSprite_->setPixel(3, -3, trouserColor);
        
        trunkSprite_->setPixel(-3, -4, trouserColor);
        trunkSprite_->setPixel(-2, -4, trouserColor);
        trunkSprite_->setPixel(2, -4, trouserColor);
        trunkSprite_->setPixel(3, -4, trouserColor);
        
        trunkSprite_->setPixel(-3, -5, bootColor);
        trunkSprite_->setPixel(-2, -5, bootColor);
        trunkSprite_->setPixel(2, -5, bootColor);
        trunkSprite_->setPixel(3, -5, bootColor);
        
        trunkSprite_->setPixel(-3, -6, bootColor);
        trunkSprite_->setPixel(-2, -6, bootColor);
        trunkSprite_->setPixel(-1, -6, bootColor);
        trunkSprite_->setPixel(2, -6, bootColor);
        trunkSprite_->setPixel(3, -6, bootColor);
        trunkSprite_->setPixel(4, -6, bootColor);
        
        headSprite_->setPosition(Vector2(0.0f, 0.25f));
        trunkSprite_->setPosition(Vector2(0.0f, -0.15f));
    }
}
