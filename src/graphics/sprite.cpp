#include "sprite.hpp"

#include <cmath>
#include <SDL/SDL_opengl.h>

namespace crust {
    Sprite::Sprite() :
        angle_(0.0f),
        scale_(1.0f),
        color_(255),

        pixels_(Color4(0, 0)),

        textureDirty_(true),
        texture_(0),
        shadowTexture_(0)
    { }

    Sprite::~Sprite()
    {
        if (texture_) {
            glDeleteTextures(1, &texture_);
        }
        if (shadowTexture_) {
            glDeleteTextures(1, &shadowTexture_);
        }
    }
    
    void Sprite::draw() const
    {
        updateTexture();
        drawTexture();
    }

    void Sprite::drawTexture() const
    {
        glPushMatrix();
        glTranslatef(position_.x, position_.y, 0.0f);
        glRotatef((180.0f / M_PI) * angle_, 0.0f, 0.0f, 1.0f);
        glScalef(scale_.x, scale_.y, 1.0f);
        glTranslatef(-0.5f, -0.5f, 0.0f);

        float x1 = float(pixels_.getX());
        float y1 = float(pixels_.getY());
        float x2 = float(pixels_.getX() + pixels_.getWidth());
        float y2 = float(pixels_.getY() + pixels_.getHeight());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowTexture_);

        glColor4ub(color_.red, color_.green, color_.blue, color_.alpha);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(x1 - 2.0f, y1 - 2.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(x2 + 2.0f, y1 - 2.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(x2 + 2.0f, y2 + 2.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(x1 - 2.0f, y2 + 2.0f);
        glEnd();

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glPopMatrix();
    }
    
    void Sprite::updateTexture() const
    {
        if (!textureDirty_) {
            return;
        }

        int x = pixels_.getX();
        int y = pixels_.getY();
        int width = pixels_.getWidth();
        int height = pixels_.getHeight();
        
        if (texture_) {
            glDeleteTextures(1, &texture_);
        }
        glGenTextures(1, &texture_);

        std::vector<unsigned char> data;
        for (int dy = -2; dy < height + 2; ++dy) {
            for (int dx = -2; dx < width + 2; ++dx) {
                Color4 const &color = pixels_.getElement(x + dx, y + dy);
                data.push_back(color.red);
                data.push_back(color.green);
                data.push_back(color.blue);
                data.push_back(color.alpha);
            }
        }
        
        glBindTexture(GL_TEXTURE_2D, texture_);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, width + 4, height + 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data.front());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        if (shadowTexture_) {
            glDeleteTextures(1, &shadowTexture_);
        }
        glGenTextures(1, &shadowTexture_);
        
        std::vector<unsigned char> shadowData;
        for (int dy = -2; dy < height + 2; ++dy) {
            for (int dx = -2; dx < width + 2; ++dx) {
                unsigned char alpha = pixels_.getElement(x + dx, y + dy).alpha;
                unsigned char neighborAlpha = std::max(std::max(pixels_.getElement(x + dx - 1, y + dy).alpha,
                                                                pixels_.getElement(x + dx + 1, y + dy).alpha),
                                                       std::max(pixels_.getElement(x + dx, y + dy - 1).alpha,
                                                                pixels_.getElement(x + dx, y + dy + 1).alpha));
                unsigned char diagonalAlpha = std::max(std::max(pixels_.getElement(x + dx - 1, y + dy - 1).alpha,
                                                                pixels_.getElement(x + dx + 1, y + dy - 1).alpha),
                                                       std::max(pixels_.getElement(x + dx + 1, y + dy + 1).alpha,
                                                                pixels_.getElement(x + dx - 1, y + dy + 1).alpha));
                unsigned char combinedAlpha = std::max(alpha / 1, std::max(neighborAlpha / 3, diagonalAlpha / 9));
                shadowData.push_back(0);
                shadowData.push_back(0);
                shadowData.push_back(0);
                shadowData.push_back(combinedAlpha);
            }
        }

        glBindTexture(GL_TEXTURE_2D, shadowTexture_);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, width + 4, height + 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, &shadowData.front());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        textureDirty_ = false;
    }
}
