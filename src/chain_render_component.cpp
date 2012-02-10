#include "chain_render_component.hpp"

#include "actor.hpp"
#include "chain_physics_component.hpp"
#include "wire.hpp"

#include <Box2D/Box2D.h>
#include <SDL/SDL_opengl.h>

namespace crust {    
    ChainRenderComponent::ChainRenderComponent(Actor *actor) :
        actor_(actor),
        physicsComponent_(wire(actor->getPhysicsComponent()))
    { }

    void ChainRenderComponent::draw() const
    {
        ChainPhysicsComponent::BodyVector const &bodies = physicsComponent_->getBodies();

        for (std::size_t i = 0; i < bodies.size(); ++i) {
            b2Vec2 position = bodies[i]->GetPosition();
            float angle = bodies[i]->GetAngle();
            glPushMatrix();
            glTranslatef(position.x, position.y, 0.0f);
            glRotatef(180.0f / M_PI * angle, 0.0f, 0.0f, 1.0f);
            glColor3ub(255, 255, 255);
            glBegin(GL_QUADS);
            glVertex2f(-0.1f, -0.1f);
            glVertex2f(0.0f, -0.1f);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(-0.1f, 0.0f);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(0.1f, 0.0f);
            glVertex2f(0.1f, 0.1f);
            glVertex2f(0.0f, 0.1f);
            glEnd();
            glColor3ub(0, 0, 0);
            glBegin(GL_LINES);
            glVertex2f(-0.1f, -0.1f);
            glVertex2f(0.0f, -0.1f);
            glVertex2f(0.0f, -0.1f);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(-0.1f, 0.0f);
            glVertex2f(-0.1f, 0.0f);
            glVertex2f(-0.1f, -0.1f);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(0.1f, 0.0f);
            glVertex2f(0.1f, 0.0f);
            glVertex2f(0.1f, 0.1f);
            glVertex2f(0.1f, 0.1f);
            glVertex2f(0.0f, 0.1f);
            glVertex2f(0.0f, 0.1f);
            glVertex2f(0.0f, 0.0f);
            glEnd();
            glPopMatrix();
        }
    }
}
