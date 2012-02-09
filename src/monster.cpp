#include "monster.hpp"

#include "debug_draw.hpp"
#include "game.hpp"
#include "geometry.hpp"
#include "monster_animation_component.hpp"
#include "monster_control_component.hpp"
#include "monster_physics_component.hpp"
#include "monster_render_component.hpp"
#include "sprite.hpp"

namespace crust {
    Monster::Monster(Game *game, Vector2 const &position) :
        game_(game)
    {
        physicsComponent_.reset(new MonsterPhysicsComponent(this, position));
        controlComponent_.reset(new MonsterControlComponent(this, physicsComponent_.get()));
        renderComponent_.reset(new MonsterRenderComponent(this));
        animationComponent_.reset(new MonsterAnimationComponent(this,
                                                                physicsComponent_.get(),
                                                                controlComponent_.get(),
                                                                renderComponent_.get()));
    }

    Monster::~Monster()
    { }

    Vector2 Monster::getPosition() const
    {
        b2Vec2 position = physicsComponent_->getMainBody()->GetPosition();
        return Vector2(position.x, position.y);
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
