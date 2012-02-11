#include "sprite.hpp"

#include <cmath>
#include <SDL/SDL_opengl.h>

namespace crust {
    void Sprite::draw() const
    {
        int x = pixels_.getX();
        int y = pixels_.getY();
        int width = pixels_.getWidth();
        int height = pixels_.getHeight();

        glPushMatrix();
        glTranslatef(position_.x, position_.y, 0.0f);
        glRotatef((180.0f / M_PI) * angle_, 0.0f, 0.0f, 1.0f);
        glScalef(scale_.x, scale_.y, 1.0f);
        glTranslatef(-0.5f, -0.5f, 0.0f);

        float halfLineWidth = 1.0f / 3.0f;
        glBegin(GL_QUADS);
        glColor3ub(0, 0, 0);
        for (int dy = 0; dy < height + 1; ++dy) {
            for (int dx = 0; dx < width + 1; ++dx) {
                bool center(pixels_.getElement(x + dx, y + dy).alpha);
                bool left(pixels_.getElement(x + dx - 1, y + dy).alpha);
                bool bottom(pixels_.getElement(x + dx, y + dy - 1).alpha);
                if (left != center) {
                    glVertex2f(float(x + dx) - halfLineWidth, float(y + dy) - halfLineWidth);
                    glVertex2f(float(x + dx) + halfLineWidth, float(y + dy) - halfLineWidth);
                    glVertex2f(float(x + dx) + halfLineWidth, float(y + dy + 1) + halfLineWidth);
                    glVertex2f(float(x + dx) - halfLineWidth, float(y + dy + 1) + halfLineWidth);
                }
                if (bottom != center) {
                    glVertex2f(float(x + dx) - halfLineWidth, float(y + dy) - halfLineWidth);
                    glVertex2f(float(x + dx + 1) + halfLineWidth, float(y + dy) - halfLineWidth);
                    glVertex2f(float(x + dx + 1) + halfLineWidth, float(y + dy) + halfLineWidth);
                    glVertex2f(float(x + dx) - halfLineWidth, float(y + dy) + halfLineWidth);
                }
            }
        }
        glEnd();
        
        glBegin(GL_QUADS);
        for (int dy = 0; dy < height; ++dy) {
            for (int dx = 0; dx < width; ++dx) {
                Color4 const &color = pixels_.getElement(x + dx, y + dy);
                if (color.alpha) {
                    glColor4ub(color.red, color.green, color.blue,
                               color.alpha);
                    glVertex2i(x + dx, y + dy);
                    glVertex2i(x + dx + 1, y + dy);
                    glVertex2i(x + dx + 1, y + dy + 1);
                    glVertex2i(x + dx, y + dy + 1);
                }
            }
        }
        glEnd();

        glPopMatrix();
    }
}
