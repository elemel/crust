#include "monster_render_component.hpp"

#include "sprite.hpp"

namespace crust {
    MonsterRenderComponent::MonsterRenderComponent(Monster *monster) :
        monster_(monster)
    {
        initSprites();
    }

    void MonsterRenderComponent::draw() const
    {
        bodySprite_->draw();
        headSprite_->draw();
    }

    
    void MonsterRenderComponent::initSprites()
    {
        headSprite_.reset(new Sprite);
        bodySprite_.reset(new Sprite);

        headSprite_->setScale(Vector2(0.1f));
        bodySprite_->setScale(Vector2(0.1f));
        
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
        
        bodySprite_->setPixel(Vector2(-0.1f, 0.4f), skinColor);
        bodySprite_->setPixel(Vector2(0.0f, 0.4f), skinColor);
        bodySprite_->setPixel(Vector2(0.1f, 0.4f), skinColor);
        
        bodySprite_->setPixel(Vector2(-0.1f, 0.3f), skinColor);
        bodySprite_->setPixel(Vector2(0.0f, 0.3f), skinColor);
        bodySprite_->setPixel(Vector2(0.1f, 0.3f), skinColor);
        
        bodySprite_->setPixel(Vector2(-0.4f, 0.2f), shirtColor);
        bodySprite_->setPixel(Vector2(-0.3f, 0.2f), shirtColor);
        bodySprite_->setPixel(Vector2(-0.2f, 0.2f), shirtColor);
        bodySprite_->setPixel(Vector2(-0.1f, 0.2f), skinColor);
        bodySprite_->setPixel(Vector2(0.0f, 0.2f), skinColor);
        bodySprite_->setPixel(Vector2(0.1f, 0.2f), skinColor);
        bodySprite_->setPixel(Vector2(0.2f, 0.2f), shirtColor);
        bodySprite_->setPixel(Vector2(0.3f, 0.2f), shirtColor);
        bodySprite_->setPixel(Vector2(0.4f, 0.2f), shirtColor);
        
        bodySprite_->setPixel(Vector2(-0.5f, 0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(-0.4f, 0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(-0.3f, 0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(-0.2f, 0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(-0.1f, 0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(0.0f, 0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(0.1f, 0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(0.2f, 0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(0.3f, 0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(0.4f, 0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(0.5f, 0.1f), shirtColor);
        
        bodySprite_->setPixel(Vector2(-0.5f, 0.0f), shirtColor);
        bodySprite_->setPixel(Vector2(-0.3f, 0.0f), shirtColor);
        bodySprite_->setPixel(Vector2(-0.2f, 0.0f), shirtColor);
        bodySprite_->setPixel(Vector2(-0.1f, 0.0f), shirtColor);
        bodySprite_->setPixel(Vector2(0.0f, 0.0f), shirtColor);
        bodySprite_->setPixel(Vector2(0.1f, 0.0f), shirtColor);
        bodySprite_->setPixel(Vector2(0.2f, 0.0f), shirtColor);
        bodySprite_->setPixel(Vector2(0.3f, 0.0f), shirtColor);
        bodySprite_->setPixel(Vector2(0.5f, 0.0f), shirtColor);
        
        bodySprite_->setPixel(Vector2(-0.5f, -0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(-0.3f, -0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(-0.2f, -0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(-0.1f, -0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(0.0f, -0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(0.1f, -0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(0.2f, -0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(0.3f, -0.1f), shirtColor);
        bodySprite_->setPixel(Vector2(0.5f, -0.1f), shirtColor);
        
        bodySprite_->setPixel(Vector2(-0.3f, -0.2f), beltColor);
        bodySprite_->setPixel(Vector2(-0.2f, -0.2f), beltColor);
        bodySprite_->setPixel(Vector2(-0.1f, -0.2f), beltColor);
        bodySprite_->setPixel(Vector2(0.0f, -0.2f), beltColor);
        bodySprite_->setPixel(Vector2(0.1f, -0.2f), beltColor);
        bodySprite_->setPixel(Vector2(0.2f, -0.2f), beltColor);
        bodySprite_->setPixel(Vector2(0.3f, -0.2f), beltColor);
        
        bodySprite_->setPixel(Vector2(0.0f, -0.2f), buckleColor);
        bodySprite_->setPixel(Vector2(0.1f, -0.2f), buckleColor);
        
        bodySprite_->setPixel(Vector2(-0.5f, -0.2f), skinColor);
        bodySprite_->setPixel(Vector2(-0.5f, -0.3f), skinColor);
        
        bodySprite_->setPixel(Vector2(0.5f, -0.2f), skinColor);
        bodySprite_->setPixel(Vector2(0.5f, -0.3f), skinColor);
        
        bodySprite_->setPixel(Vector2(-0.3f, -0.3f), trouserColor);
        bodySprite_->setPixel(Vector2(-0.2f, -0.3f), trouserColor);
        bodySprite_->setPixel(Vector2(-0.1f, -0.3f), trouserColor);
        bodySprite_->setPixel(Vector2(0.0f, -0.3f), trouserColor);
        bodySprite_->setPixel(Vector2(0.1f, -0.3f), trouserColor);
        bodySprite_->setPixel(Vector2(0.2f, -0.3f), trouserColor);
        bodySprite_->setPixel(Vector2(0.3f, -0.3f), trouserColor);
        
        bodySprite_->setPixel(Vector2(-0.3f, -0.4f), trouserColor);
        bodySprite_->setPixel(Vector2(-0.2f, -0.4f), trouserColor);
        bodySprite_->setPixel(Vector2(0.2f, -0.4f), trouserColor);
        bodySprite_->setPixel(Vector2(0.3f, -0.4f), trouserColor);
        
        bodySprite_->setPixel(Vector2(-0.3f, -0.5f), bootColor);
        bodySprite_->setPixel(Vector2(-0.2f, -0.5f), bootColor);
        bodySprite_->setPixel(Vector2(0.2f, -0.5f), bootColor);
        bodySprite_->setPixel(Vector2(0.3f, -0.5f), bootColor);
        
        bodySprite_->setPixel(Vector2(-0.3f, -0.6f), bootColor);
        bodySprite_->setPixel(Vector2(-0.2f, -0.6f), bootColor);
        bodySprite_->setPixel(Vector2(-0.1f, -0.6f), bootColor);
        bodySprite_->setPixel(Vector2(0.2f, -0.6f), bootColor);
        bodySprite_->setPixel(Vector2(0.3f, -0.6f), bootColor);
        bodySprite_->setPixel(Vector2(0.4f, -0.6f), bootColor);
        
        headSprite_->setPosition(Vector2(0.0f, 0.25f));
        bodySprite_->setPosition(Vector2(0.0f, -0.15f));
    }
}
