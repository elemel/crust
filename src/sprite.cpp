#include "sprite.hpp"

#include <cmath>
#include <SDL/SDL_opengl.h>

namespace crust {
    void Sprite::setPixel(Vector2 const &position, Color4 const &color)
    {
        Matrix3 transform;
        transform.translate(position_);
        transform.rotate(angle_);
        transform.scale(scale_);
        transform.invert();
        Vector2 localPosition = transformPoint(transform, position);
        int x = int(std::floor(localPosition.x + 0.5f));
        int y = int(std::floor(localPosition.y + 0.5f));
        pixels_.setElement(x, y, color);
    }

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

        glBegin(GL_LINES);
        glColor3ub(0, 0, 0);
        for (int dy = 0; dy < height + 1; ++dy) {
            for (int dx = 0; dx < width + 1; ++dx) {
                bool center(pixels_.getElement(x + dx, y + dy).alpha);
                bool left(pixels_.getElement(x + dx - 1, y + dy).alpha);
                bool bottom(pixels_.getElement(x + dx, y + dy - 1).alpha);
                if (center != left) {
                    glVertex2i(x + dx, y + dy);
                    glVertex2i(x + dx, y + dy + 1);
                }
                if (center != bottom) {
                    glVertex2i(x + dx, y + dy);
                    glVertex2i(x + dx + 1, y + dy);
                }
            }
        }
        glEnd();
        
        glPopMatrix();
    }
}
