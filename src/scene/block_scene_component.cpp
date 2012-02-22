#include "block_scene_component.hpp"

#include "actor.hpp"
#include "block_physics_component.hpp"
#include "convert.hpp"
#include "game.hpp"
#include "grid.hpp"
#include "hash.hpp"
#include "scene_service.hpp"
#include "sprite.hpp"

namespace crust {
    BlockSceneComponent::BlockSceneComponent(Actor *actor) :
        actor_(actor),
        physicsComponent_(convert(actor->getPhysicsComponent())),
        sceneService_(actor->getGame()->getSceneService())
    { }

    BlockSceneComponent::~BlockSceneComponent()
    { }

    void BlockSceneComponent::create()
    {
        initSprite();
        sceneService_->addSprite(sprite_.get());
        sceneService_->addTask(this);
    }
    
    void BlockSceneComponent::destroy()
    {
        sceneService_->removeTask(this);
        sceneService_->removeSprite(sprite_.get());
    }
    
    void BlockSceneComponent::step(float dt)
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
    
    void BlockSceneComponent::initSprite()
    {
        Grid<unsigned char> const &grid = physicsComponent_->getGrid();

        sprite_.reset(new Sprite);
        sprite_->setScale(Vector2(0.1f));

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
    }
}
