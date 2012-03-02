#include "block_graphics_component.hpp"

#include "actor.hpp"
#include "block_physics_component.hpp"
#include "color_generator.hpp"
#include "convert.hpp"
#include "game.hpp"
#include "graphics_manager.hpp"
#include "grid.hpp"
#include "hash.hpp"
#include "random.hpp"
#include "sprite.hpp"

namespace crust {
    BlockGraphicsComponent::BlockGraphicsComponent(Actor *actor) :
        actor_(actor),
        physicsComponent_(convert(actor->getPhysicsComponent())),
        graphicsManager_(actor->getGame()->getGraphicsManager())
    { }

    BlockGraphicsComponent::~BlockGraphicsComponent()
    { }

    void BlockGraphicsComponent::create()
    {
        initSprite();
        graphicsManager_->addSprite(sprite_.get());
        graphicsManager_->addTask(this);
    }
    
    void BlockGraphicsComponent::destroy()
    {
        graphicsManager_->removeTask(this);
        graphicsManager_->removeSprite(sprite_.get());
    }
    
    void BlockGraphicsComponent::step(float dt)
    {
        float duration = physicsComponent_->getMineDuration();

        b2Vec2 position = physicsComponent_->getBody()->GetPosition();
        float angle = physicsComponent_->getBody()->GetAngle();
        position.x += 0.03f * std::sin(30.0f * duration);
        position.y += 0.03f * std::sin(40.0f * duration);
        angle += 0.03f * std::sin(50.0f * duration);
        sprite_->setPosition(Vector2(position.x, position.y));
        sprite_->setAngle(angle);
    }
    
    void BlockGraphicsComponent::initSprite()
    {
        Grid<unsigned char> const &grid = physicsComponent_->getGrid();

        sprite_.reset(new Sprite);
        sprite_->setScale(Vector2(0.1f));

        int x = grid.getX();
        int y = grid.getY();
        int width = grid.getWidth();
        int height = grid.getHeight();

        ColorGenerator colorGenerator(actor_->getGame()->getRandom());
        
        for (int dy = 0; dy < height; ++dy) {
            for (int dx = 0; dx < width; ++dx) {
                int type = grid.getElement(x + dx, y + dy);
                if (type) {
                    Color3 color = colorGenerator.generateColor();
                    sprite_->setPixel(x + dx, y + dy, Color4(color.red, color.green, color.blue));
                }
            }
        }
    }
}
