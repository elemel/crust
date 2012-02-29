#include "sprite.hpp"

#include <cassert>
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
        normalAndShadowTexture_(0),

        arraysDirty_(true)
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
        updateArrays();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorTexture_);        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalAndShadowTexture_);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, vertexArray_);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, texCoordArray_);
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorArray_);

        glDrawArrays(GL_QUADS, 0, 4);

        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
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

        std::vector<GLubyte> data;
        for (int dy = -2; dy < height + 2; ++dy) {
            for (int dx = -2; dx < width + 2; ++dx) {
                Color4 const &color = pixels_.getElement(x + dx, y + dy);
                data.push_back(color.red * color.alpha / 255);
                data.push_back(color.green * color.alpha / 255);
                data.push_back(color.blue * color.alpha / 255);
                data.push_back(color.alpha);
            }
        }
        
        glBindTexture(GL_TEXTURE_2D, colorTexture_);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, width + 2 * 2, height + 2 * 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data.front());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        if (normalAndShadowTexture_) {
            glDeleteTextures(1, &normalAndShadowTexture_);
        }
        glGenTextures(1, &normalAndShadowTexture_);
        
        std::vector<GLbyte> normalAndShadowData;
        for (int dy = -4; dy < 2 * height + 4; ++dy) {
            for (int dx = -4; dx < 2 * width + 4; ++dx) {

                // TODO: Normals.
                normalAndShadowData.push_back(0);
                normalAndShadowData.push_back(0);
                normalAndShadowData.push_back(127);
                
                float shadow = 0.0f;
                Vector2 position = Vector2(0.5f * float(dx), 0.5f * float(dy));
                for (int ddy = 0; ddy < 6; ++ddy) {
                    for (int ddx = 0; ddx < 6; ++ddx) {
                        if (ddy * ddy + ddx * ddx < 6 * 6) {
                            Vector2 samplePosition = position + Vector2(0.5f * float(ddx) - 1.25f, 0.5f * float(ddy) - 1.25f);
                            float alpha = float(pixels_.getElement(x + int(samplePosition.x + 1.5f) - 1,
                                                                   y + int(samplePosition.y + 1.5f) - 1).alpha) / 255.0f;
                            float minSquaredDistance = std::min(std::min(getSquaredDistance(position,
                                                                                            samplePosition + Vector2(-0.25, -0.25f)),
                                                                         getSquaredDistance(position,
                                                                                            samplePosition + Vector2(0.25, -0.25f))),
                                                                std::min(getSquaredDistance(position,
                                                                                            samplePosition + Vector2(0.25, 0.25f)),
                                                                         getSquaredDistance(position,
                                                                                            samplePosition + Vector2(-0.25, 0.25f))));
                            shadow = std::max(shadow, alpha / std::max(1.0f, 10.0f * minSquaredDistance));
                        }
                    }
                }
                shadow = 0.7f * shadow;
                normalAndShadowData.push_back(std::min(127, int(shadow * 128.0)));
            }
        }

        glBindTexture(GL_TEXTURE_2D, normalAndShadowTexture_);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, 2 * (width + 4), 2 * (height + 4), 0, GL_RGBA, GL_BYTE, &normalAndShadowData.front());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        texturesDirty_ = false;
    }

    void Sprite::updateArrays() const
    {
        if (!arraysDirty_) {
            return;
        }

        Matrix3 transform;
        transform.translate(position_);
        transform.rotate(angle_);
        transform.scale(scale_);
        transform.translate(Vector2(-0.5f) - anchor_);

        float x1 = float(pixels_.getX() - 2);
        float y1 = float(pixels_.getY() - 2);
        float x2 = float(pixels_.getX() + pixels_.getWidth() + 2);
        float y2 = float(pixels_.getY() + pixels_.getHeight() + 2);

        Vector2 vertices[] = {
            transformPoint(transform, Vector2(x1, y1)),
            transformPoint(transform, Vector2(x2, y1)),
            transformPoint(transform, Vector2(x2, y2)),
            transformPoint(transform, Vector2(x1, y2))
        };

        for (int i = 0; i < 4; ++i) {
            vertexArray_[i * 2 + 0] = vertices[i].x;
            vertexArray_[i * 2 + 1] = vertices[i].y;
        }

        texCoordArray_[0] = 0.0f;
        texCoordArray_[1] = 0.0f;
        texCoordArray_[2] = 1.0f;
        texCoordArray_[3] = 0.0f;
        texCoordArray_[4] = 1.0f;
        texCoordArray_[5] = 1.0f;
        texCoordArray_[6] = 0.0f;
        texCoordArray_[7] = 1.0f;

        for (int i = 0; i < 4; ++i) {
            colorArray_[i * 4 + 0] = color_.red;
            colorArray_[i * 4 + 1] = color_.green;
            colorArray_[i * 4 + 2] = color_.blue;
            colorArray_[i * 4 + 3] = color_.alpha;
        }

        arraysDirty_ = false;
    }
}
