#include "chain_scene_component.hpp"

#include "actor.hpp"
#include "chain_physics_component.hpp"
#include "convert.hpp"

#include <Box2D/Box2D.h>
#include <SDL/SDL_opengl.h>

namespace crust {    
    ChainSceneComponent::ChainSceneComponent(Actor *actor) :
        actor_(actor),
        physicsComponent_(convert(actor->getPhysicsComponent()))
    { }

    void ChainSceneComponent::draw()
    {
        ChainPhysicsComponent::BodyVector const &bodies = physicsComponent_->getBodies();

        float halfLineWidth = 0.1f / 3.0f;
        for (std::size_t i = 0; i < bodies.size(); ++i) {
            b2Vec2 position = bodies[i]->GetPosition();
            float angle = bodies[i]->GetAngle();
            glPushMatrix();
            glTranslatef(position.x, position.y, 0.0f);
            glRotatef(180.0f / M_PI * angle, 0.0f, 0.0f, 1.0f);

            glBegin(GL_QUADS);

            glColor3ub(0, 0, 0);
            glVertex2f(-0.1f - halfLineWidth, -0.1f - halfLineWidth);
            glVertex2f(0.0f + halfLineWidth, -0.1f - halfLineWidth);
            glVertex2f(0.0f + halfLineWidth, 0.0f + halfLineWidth);
            glVertex2f(-0.1f - halfLineWidth, 0.0f + halfLineWidth);
            glVertex2f(0.0f - halfLineWidth, 0.0f - halfLineWidth);
            glVertex2f(0.1f + halfLineWidth, 0.0f - halfLineWidth);
            glVertex2f(0.1f + halfLineWidth, 0.1f + halfLineWidth);
            glVertex2f(0.0f - halfLineWidth, 0.1f + halfLineWidth);

            glColor3ub(255, 255, 255);
            glVertex2f(-0.1f, -0.1f);
            glVertex2f(0.0f, -0.1f);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(-0.1f, 0.0f);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(0.1f, 0.0f);
            glVertex2f(0.1f, 0.1f);
            glVertex2f(0.0f, 0.1f);
            glEnd();

            glPopMatrix();
        }
    }
}
