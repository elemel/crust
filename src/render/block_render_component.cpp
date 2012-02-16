#include "block_render_component.hpp"

#include "actor.hpp"
#include "block_physics_component.hpp"
#include "convert.hpp"
#include "game.hpp"
#include "grid.hpp"
#include "hash.hpp"
#include "sprite.hpp"

namespace crust {
    BlockRenderComponent::BlockRenderComponent(Actor *actor) :
        actor_(actor),
        physicsComponent_(convert(actor->getPhysicsComponent())),

        spriteDirty_(true)
    { }

    BlockRenderComponent::~BlockRenderComponent()
    { }

    void BlockRenderComponent::draw() const
    {
        updateSprite();
        
        Vector2 position = physicsComponent_->getPosition();
        float angle = physicsComponent_->getAngle();

        glPushMatrix();
        glTranslatef(position.x, position.y, 0.0f);
        glRotatef(angle * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);
        glScalef(0.1f, 0.1f, 1.0f);
        glTranslatef(-0.5f, -0.5f, 0.0f);

        sprite_->draw();

        glPopMatrix();
    }
    
    void BlockRenderComponent::updateSprite() const
    {
        if (!spriteDirty_) {
            return;
        }
        
        Grid<unsigned char> const &grid = physicsComponent_->getGrid();

        sprite_.reset(new Sprite);

        int x = grid.getX();
        int y = grid.getY();
        int width = grid.getWidth();
        int height = grid.getHeight();

        int salt = actor_->getGame()->getRandomInt(256);
        int red0 = actor_->getGame()->getRandomInt(64);
        int green0 = actor_->getGame()->getRandomInt(64);
        int blue0 = actor_->getGame()->getRandomInt(64);
        
        for (int dy = 0; dy < height; ++dy) {
            for (int dx = 0; dx < width; ++dx) {
                int type = grid.getElement(x + dx, y + dy);
                if (type) {
                    unsigned char red = red0 + 64 + hashValue((salt << 24) + (dx << 16) + (dy << 8) + 0) % 64;
                    unsigned char green = green0 + 0 + hashValue((salt << 24) + (dx << 16) + (dy << 8) + 1) % 64;
                    unsigned char blue = blue0 + 0 + hashValue((salt << 24) + (dx << 16) + (dy << 8) + 2) % 64;
                    
                    sprite_->setPixel(x + dx, y + dy, Color4(red, green, blue));
                }
            }
        }

        spriteDirty_ = false;
    }
}
