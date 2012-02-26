#include "sprite.hpp"

#include <cmath>
#include <SDL/SDL_opengl.h>

namespace crust {
    Sprite::Sprite() :
        angle_(0.0f),
        scale_(1.0f),
        color_(255),

        pixels_(Color4(0, 0)),

        texturesDirty_(true),
        colorTexture_(0),
        normalAndShadowTexture_(0)
    { }

    Sprite::~Sprite()
    {
        if (colorTexture_) {
            glDeleteTextures(1, &colorTexture_);
        }
        if (normalAndShadowTexture_) {
            glDeleteTextures(1, &normalAndShadowTexture_);
        }
    }
    
    void Sprite::draw() const
    {
        updateTextures();
        drawTextures();
    }

    void Sprite::drawTextures() const
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
        glBindTexture(GL_TEXTURE_2D, colorTexture_);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalAndShadowTexture_);

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
    
    void Sprite::updateTextures() const
    {
        if (!texturesDirty_) {
            return;
        }

        int x = pixels_.getX();
        int y = pixels_.getY();
        int width = pixels_.getWidth();
        int height = pixels_.getHeight();
        
        if (colorTexture_) {
            glDeleteTextures(1, &colorTexture_);
        }
        glGenTextures(1, &colorTexture_);

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
        
        glBindTexture(GL_TEXTURE_2D, colorTexture_);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, width + 4, height + 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data.front());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        if (normalAndShadowTexture_) {
            glDeleteTextures(1, &normalAndShadowTexture_);
        }
        glGenTextures(1, &normalAndShadowTexture_);
        
        std::vector<signed char> normalAndShadowData;
        for (int dy = -2; dy < height + 2; ++dy) {
            for (int dx = -2; dx < width + 2; ++dx) {
                // TODO: Normals.
                normalAndShadowData.push_back(0);
                normalAndShadowData.push_back(0);
                normalAndShadowData.push_back(0);

                unsigned char shadow = pixels_.getElement(x + dx, y + dy).alpha;
                unsigned char neighborShadow = std::max(std::max(pixels_.getElement(x + dx - 1, y + dy).alpha,
                                                                 pixels_.getElement(x + dx + 1, y + dy).alpha),
                                                        std::max(pixels_.getElement(x + dx, y + dy - 1).alpha,
                                                                 pixels_.getElement(x + dx, y + dy + 1).alpha));
                unsigned char diagonalShadow = std::max(std::max(pixels_.getElement(x + dx - 1, y + dy - 1).alpha,
                                                                 pixels_.getElement(x + dx + 1, y + dy - 1).alpha),
                                                        std::max(pixels_.getElement(x + dx + 1, y + dy + 1).alpha,
                                                                 pixels_.getElement(x + dx - 1, y + dy + 1).alpha));
                unsigned char combinedShadow = std::max(shadow / 1, std::max(neighborShadow / 3, diagonalShadow / 9));
                normalAndShadowData.push_back(combinedShadow / 2);
            }
        }

        glBindTexture(GL_TEXTURE_2D, normalAndShadowTexture_);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, width + 4, height + 4, 0, GL_RGBA, GL_BYTE, &normalAndShadowData.front());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        texturesDirty_ = false;
    }
}
