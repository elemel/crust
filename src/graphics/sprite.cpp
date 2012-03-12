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
        arraysDirty_(true)
    { }

    void Sprite::draw() const
    {
        updateTextures();
        updateArrays();

        glActiveTexture(GL_TEXTURE0);
        colorTexture_.bind();
        glActiveTexture(GL_TEXTURE1);
        normalAndShadowTexture_.bind();

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
        normalAndShadowTexture_.unbind();
        glActiveTexture(GL_TEXTURE0);
        colorTexture_.unbind();
    }
    
    void Sprite::updateTextures() const
    {
        if (texturesDirty_) {
            updateColorTexture();
            updateNormalAndShadowTexture();
            texturesDirty_ = false;
        }
    }

    void Sprite::updateColorTexture() const
    {
        colorTexture_.destroy();

        int x = pixels_.getX();
        int y = pixels_.getY();
        int width = pixels_.getWidth();
        int height = pixels_.getHeight();
        
        std::vector<GLubyte> pixels;
        for (int dy = -2; dy < height + 2; ++dy) {
            for (int dx = -2; dx < width + 2; ++dx) {
                Color4 color = pixels_.getElement(x + dx, y + dy);
                pixels.push_back(color.red * color.alpha / 255);
                pixels.push_back(color.green * color.alpha / 255);
                pixels.push_back(color.blue * color.alpha / 255);
                pixels.push_back(color.alpha);
            }
        }

        colorTexture_.setInternalFormat(GL_SRGB_ALPHA);
        colorTexture_.setSize(width + 4, height + 4);
        colorTexture_.setPixels(&pixels.front(), pixels.size());
        colorTexture_.create();
    }

    void Sprite::updateNormalAndShadowTexture() const
    {
        normalAndShadowTexture_.destroy();

        int x = pixels_.getX();
        int y = pixels_.getY();
        int width = pixels_.getWidth();
        int height = pixels_.getHeight();
        
        std::vector<float> shadowData;
        for (int dy = -4; dy < 2 * height + 4; ++dy) {
            for (int dx = -4; dx < 2 * width + 4; ++dx) {
                float minShadow = 1.0f;
                float maxShadow = 0.0f;
                Vector2 position = Vector2(0.5f * float(dx), 0.5f * float(dy));
                for (int ddy = 0; ddy < 2; ++ddy) {
                    for (int ddx = 0; ddx < 2; ++ddx) {
                        Vector2 samplePosition = position + Vector2(0.5f * float(ddx) - 0.25f, 0.5f * float(ddy) - 0.25f);
                        float alpha = float(pixels_.getElement(x + int(samplePosition.x + 1.5f) - 1,
                                                               y + int(samplePosition.y + 1.5f) - 1).alpha) / 255.0f;
                        minShadow = std::min(minShadow, alpha);
                        maxShadow = std::max(maxShadow, alpha);
                    }
                }
                shadowData.push_back(minShadow < 0.001f ? maxShadow : 0.0f);
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
                }
            }
        }
        
        std::vector<GLbyte> pixels(4 * smoothShadowData.size());
        for (std::size_t i = 0; i < smoothShadowData.size(); ++i) {
            pixels[4 * i + 0] = 0;
            pixels[4 * i + 1] = 0;
            pixels[4 * i + 2] = 127;
            pixels[4 * i + 3] = std::min(127, int(smoothShadowData[i] * 128.0));
        }
        
        normalAndShadowTexture_.setInternalFormat(GL_SRGB_ALPHA);
        normalAndShadowTexture_.setSize(2 * (width + 4), 2 * (height + 4));
        normalAndShadowTexture_.setType(GL_BYTE);
        normalAndShadowTexture_.setPixels(&pixels.front(), pixels.size());
        normalAndShadowTexture_.create();
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
        float texWidth = 1.0f / width;
        float texHeight = 1.0f / height;
        
        Vector2 vertices[] = {
            transformPoint(transform, Vector2(x1 + 1.0f, y1 + 1.0f)),
            transformPoint(transform, Vector2(x2 - 1.0f, y1 + 1.0f)),
            transformPoint(transform, Vector2(x2 - 1.0f, y2 - 1.0f)),
            transformPoint(transform, Vector2(x1 + 1.0f, y2 - 1.0f))
        };

        for (int i = 0; i < 4; ++i) {
            vertexArray_[i * 2 + 0] = vertices[i].x;
            vertexArray_[i * 2 + 1] = vertices[i].y;
        }

        texCoordArray_[0] = 0.0f + texWidth;
        texCoordArray_[1] = 0.0f + texHeight;
        texCoordArray_[2] = 1.0f - texWidth;
        texCoordArray_[3] = 0.0f + texHeight;
        texCoordArray_[4] = 1.0f - texWidth;
        texCoordArray_[5] = 1.0f - texHeight;
        texCoordArray_[6] = 0.0f + texWidth;
        texCoordArray_[7] = 1.0f - texHeight;
        
        for (int i = 0; i < 4; ++i) {
            colorArray_[i * 4 + 0] = color_.red;
            colorArray_[i * 4 + 1] = color_.green;
            colorArray_[i * 4 + 2] = color_.blue;
            colorArray_[i * 4 + 3] = color_.alpha;
        }

        arraysDirty_ = false;
    }
}
