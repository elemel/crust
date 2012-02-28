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

        std::vector<unsigned char> data;
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
        glTexImage2D(GL_TEXTURE_2D, 0, 4, width + 4, height + 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data.front());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
                normalAndShadowData.push_back(127);

                unsigned char shadow = pixels_.getElement(x + dx, y + dy).alpha;
                unsigned char neighborShadow = std::max(std::max(pixels_.getElement(x + dx - 1, y + dy).alpha,
                                                                 pixels_.getElement(x + dx + 1, y + dy).alpha),
                                                        std::max(pixels_.getElement(x + dx, y + dy - 1).alpha,
                                                                 pixels_.getElement(x + dx, y + dy + 1).alpha));
                unsigned char diagonalShadow = std::max(std::max(pixels_.getElement(x + dx - 1, y + dy - 1).alpha,
                                                                 pixels_.getElement(x + dx + 1, y + dy - 1).alpha),
                                                        std::max(pixels_.getElement(x + dx + 1, y + dy + 1).alpha,
                                                                 pixels_.getElement(x + dx - 1, y + dy + 1).alpha));
                unsigned char combinedShadow = std::max(shadow / 1, std::max(neighborShadow / 2, diagonalShadow / 4));
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
