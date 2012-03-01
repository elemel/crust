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

        std::vector<float> shadowData;
        for (int dy = -4; dy < 2 * height + 4; ++dy) {
            for (int dx = -4; dx < 2 * width + 4; ++dx) {
                float shadow = 0.0f;
                Vector2 position = Vector2(0.5f * float(dx), 0.5f * float(dy));
                for (int ddy = 0; ddy < 2; ++ddy) {
                    for (int ddx = 0; ddx < 2; ++ddx) {
                        Vector2 samplePosition = position + Vector2(0.5f * float(ddx) - 0.25f, 0.5f * float(ddy) - 0.25f);
                        float alpha = float(pixels_.getElement(x + int(samplePosition.x + 1.5f) - 1,
                                                               y + int(samplePosition.y + 1.5f) - 1).alpha) / 255.0f;
                        shadow = std::max(shadow, alpha);
                    }
                }
                shadowData.push_back(0.8f * shadow);
            }
        }

        std::vector<float> smoothShadowData(shadowData.size(), 0.0f);
        int pitch = 2 * width + 8;
        for (int dy = 2; dy < 2 * height + 8 - 2; ++dy) {
            for (int dx = 2; dx < 2 * width + 8 - 2; ++dx) {
                float shadow = shadowData[dy * pitch + dx];
                if (0.001f < shadow) {
                    smoothShadowData[dy * pitch + dx] = std::max(shadow, smoothShadowData[dy * pitch + dx]);

                    smoothShadowData[dy * pitch + dx + 1] = std::max(0.5f * shadow, smoothShadowData[dy * pitch + dx + 1]);
                    smoothShadowData[dy * pitch + dx - 1] = std::max(0.5f * shadow, smoothShadowData[dy * pitch + dx - 1]);
                    smoothShadowData[(dy + 1) * pitch + dx] = std::max(0.5f * shadow, smoothShadowData[(dy + 1) * pitch + dx]);
                    smoothShadowData[(dy - 1) * pitch + dx] = std::max(0.5f * shadow, smoothShadowData[(dy - 1) * pitch + dx]);

                    smoothShadowData[(dy - 1) * pitch + dx - 1] = std::max(0.3f * shadow, smoothShadowData[(dy - 1) * pitch + dx - 1]);
                    smoothShadowData[(dy - 1) * pitch + dx + 1] = std::max(0.3f * shadow, smoothShadowData[(dy - 1) * pitch + dx + 1]);
                    smoothShadowData[(dy + 1) * pitch + dx + 1] = std::max(0.3f * shadow, smoothShadowData[(dy + 1) * pitch + dx + 1]);
                    smoothShadowData[(dy + 1) * pitch + dx - 1] = std::max(0.3f * shadow, smoothShadowData[(dy + 1) * pitch + dx - 1]);

                    smoothShadowData[dy * pitch + dx + 2] = std::max(0.2f * shadow, smoothShadowData[dy * pitch + dx + 2]);
                    smoothShadowData[dy * pitch + dx - 2] = std::max(0.2f * shadow, smoothShadowData[dy * pitch + dx - 2]);
                    smoothShadowData[(dy + 2) * pitch + dx] = std::max(0.2f * shadow, smoothShadowData[(dy + 2) * pitch + dx]);
                    smoothShadowData[(dy - 2) * pitch + dx] = std::max(0.2f * shadow, smoothShadowData[(dy - 2) * pitch + dx]);

                    smoothShadowData[(dy - 1) * pitch + dx - 2] = std::max(0.1f * shadow, smoothShadowData[(dy - 1) * pitch + dx - 2]);
                    smoothShadowData[(dy - 1) * pitch + dx + 2] = std::max(0.1f * shadow, smoothShadowData[(dy - 1) * pitch + dx + 2]);
                    smoothShadowData[(dy + 1) * pitch + dx + 2] = std::max(0.1f * shadow, smoothShadowData[(dy + 1) * pitch + dx + 2]);
                    smoothShadowData[(dy + 1) * pitch + dx - 2] = std::max(0.1f * shadow, smoothShadowData[(dy + 1) * pitch + dx - 2]);

                    smoothShadowData[(dy - 2) * pitch + dx - 1] = std::max(0.1f * shadow, smoothShadowData[(dy - 2) * pitch + dx - 1]);
                    smoothShadowData[(dy - 2) * pitch + dx + 1] = std::max(0.1f * shadow, smoothShadowData[(dy - 2) * pitch + dx + 1]);
                    smoothShadowData[(dy + 2) * pitch + dx + 1] = std::max(0.1f * shadow, smoothShadowData[(dy + 2) * pitch + dx + 1]);
                    smoothShadowData[(dy + 2) * pitch + dx - 1] = std::max(0.1f * shadow, smoothShadowData[(dy + 2) * pitch + dx - 1]);
                }
            }
        }

        std::vector<GLbyte> normalAndShadowData(4 * smoothShadowData.size());
        for (std::size_t i = 0; i < smoothShadowData.size(); ++i) {
            normalAndShadowData[4 * i + 0] = 0;
            normalAndShadowData[4 * i + 1] = 0;
            normalAndShadowData[4 * i + 2] = 127;
            normalAndShadowData[4 * i + 3] = std::min(127, int(smoothShadowData[i] * 128.0));
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
        float width = x2 - x1;
        float height = y2 - y1;
        float halfTexWidth = 0.5f / width;
        float halfTexHeight = 0.5f / height;
        
        Vector2 vertices[] = {
            transformPoint(transform, Vector2(x1 + 0.5f, y1 + 0.5f)),
            transformPoint(transform, Vector2(x2 - 0.5f, y1 + 0.5f)),
            transformPoint(transform, Vector2(x2 - 0.5f, y2 - 0.5f)),
            transformPoint(transform, Vector2(x1 + 0.5f, y2 - 0.5f))
        };

        for (int i = 0; i < 4; ++i) {
            vertexArray_[i * 2 + 0] = vertices[i].x;
            vertexArray_[i * 2 + 1] = vertices[i].y;
        }

        texCoordArray_[0] = 0.0f + halfTexWidth;
        texCoordArray_[1] = 0.0f + halfTexHeight;
        texCoordArray_[2] = 1.0f - halfTexWidth;
        texCoordArray_[3] = 0.0f + halfTexHeight;
        texCoordArray_[4] = 1.0f - halfTexWidth;
        texCoordArray_[5] = 1.0f - halfTexHeight;
        texCoordArray_[6] = 0.0f + halfTexWidth;
        texCoordArray_[7] = 1.0f - halfTexHeight;
        
        for (int i = 0; i < 4; ++i) {
            colorArray_[i * 4 + 0] = color_.red;
            colorArray_[i * 4 + 1] = color_.green;
            colorArray_[i * 4 + 2] = color_.blue;
            colorArray_[i * 4 + 3] = color_.alpha;
        }

        arraysDirty_ = false;
    }
}
