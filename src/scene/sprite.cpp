#include "sprite.hpp"

#include <cmath>
#include <SDL/SDL_opengl.h>

namespace crust {
    Sprite::Sprite(Vector2 const &position, float angle, float scale) :
        position_(position),
        angle_(angle),
        scale_(scale),

        pixels_(Color4(0, 0)),

        verticesDirty_(false),

        bufferDirty_(false),
        bufferName_(0),
        bufferCount_(0)
    { }

    Sprite::~Sprite()
    {
        if (bufferName_) {
            glDeleteBuffers(1, &bufferName_);
        }
    }
    
    void Sprite::draw() const
    {
        updateVertices();
        updateBuffer();
        drawBuffer();
        // drawVertices();
        // drawDirectMode();
    }

    void Sprite::drawDirectMode() const
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

    void Sprite::drawVertices() const
    {
        glPushMatrix();
        glTranslatef(position_.x, position_.y, 0.0f);
        glRotatef((180.0f / M_PI) * angle_, 0.0f, 0.0f, 1.0f);
        glScalef(scale_.x, scale_.y, 1.0f);
        glTranslatef(-0.5f, -0.5f, 0.0f);

        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        
        if (!vertices_.empty()) {
            glColorPointer(3, GL_FLOAT, sizeof(Vertex), &vertices_[0].red);
            glNormalPointer(GL_FLOAT, sizeof(Vertex), &vertices_[0].normalX);
            glVertexPointer(2, GL_FLOAT, sizeof(Vertex), &vertices_[0].x);
            glDrawArrays(GL_QUADS, 0, GLsizei(vertices_.size()));
        }
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);

        glPopMatrix();
    }

    void Sprite::drawBuffer() const
    {
        glPushMatrix();
        glTranslatef(position_.x, position_.y, 0.0f);
        glRotatef((180.0f / M_PI) * angle_, 0.0f, 0.0f, 1.0f);
        glScalef(scale_.x, scale_.y, 1.0f);
        glTranslatef(-0.5f, -0.5f, 0.0f);

        glBindBuffer(GL_ARRAY_BUFFER, bufferName_);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);

        unsigned char *null = 0;
        glColorPointer(3, GL_FLOAT, sizeof(Vertex), null);
        glNormalPointer(GL_FLOAT, sizeof(Vertex), null + sizeof(GLfloat) * 3);
        glVertexPointer(2, GL_FLOAT, sizeof(Vertex), null + sizeof(GLfloat) * 6);

        glDrawArrays(GL_QUADS, 0, bufferCount_);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glPopMatrix();
    }
    
    void Sprite::updateVertices() const
    {
        if (!verticesDirty_) {
            return;
        }

        vertices_.clear();

        int x = pixels_.getX();
        int y = pixels_.getY();
        int width = pixels_.getWidth();
        int height = pixels_.getHeight();

        float halfLineWidth = 1.0f / 3.0f;

        for (int dy = 0; dy < height + 1; ++dy) {
            for (int dx = 0; dx < width + 1; ++dx) {
                Vertex vertex;

                vertex.red = 0.0f;
                vertex.green = 0.0f;
                vertex.blue = 0.0f;
 
                vertex.normalX = 0.0f;
                vertex.normalY = 0.0f;
                vertex.normalZ = 1.0f;
                
                vertex.x = 0.0f;
                vertex.y = 0.0f;

                bool center = bool(pixels_.getElement(x + dx, y + dy).alpha);
                bool left = bool(pixels_.getElement(x + dx - 1, y + dy).alpha);
                bool bottom = bool(pixels_.getElement(x + dx, y + dy - 1).alpha);
                
                if (center != left) {
                    vertices_.push_back(vertex);
                    vertices_.back().x = float(x + dx) - halfLineWidth;
                    vertices_.back().y = float(y + dy) - halfLineWidth;
                    vertices_.push_back(vertex);
                    vertices_.back().x = float(x + dx) + halfLineWidth;
                    vertices_.back().y = float(y + dy) - halfLineWidth;
                    vertices_.push_back(vertex);
                    vertices_.back().x = float(x + dx) + halfLineWidth;
                    vertices_.back().y = float(y + dy + 1) + halfLineWidth;
                    vertices_.push_back(vertex);
                    vertices_.back().x = float(x + dx) - halfLineWidth;
                    vertices_.back().y = float(y + dy + 1) + halfLineWidth;
                }

                if (center != bottom) {
                    vertices_.push_back(vertex);
                    vertices_.back().x = float(x + dx) - halfLineWidth;
                    vertices_.back().y = float(y + dy) - halfLineWidth;
                    vertices_.push_back(vertex);
                    vertices_.back().x = float(x + dx + 1) + halfLineWidth;
                    vertices_.back().y = float(y + dy) - halfLineWidth;
                    vertices_.push_back(vertex);
                    vertices_.back().x = float(x + dx + 1) + halfLineWidth;
                    vertices_.back().y = float(y + dy) + halfLineWidth;
                    vertices_.push_back(vertex);
                    vertices_.back().x = float(x + dx) - halfLineWidth;
                    vertices_.back().y = float(y + dy) + halfLineWidth;
                }
            }
        }

        for (int dy = 0; dy < height; ++dy) {
            for (int dx = 0; dx < width; ++dx) {
                Color4 const &color = pixels_.getElement(x + dx, y + dy);
                if (color.alpha) {
                    Vertex vertex;
                    vertex.red = float(color.red) / 255.0f;
                    vertex.green = float(color.green) / 255.0f;
                    vertex.blue = float(color.blue) / 255.0f;
                    
                    vertex.normalX = 0.0f;
                    vertex.normalY = 0.0f;
                    vertex.normalZ = 1.0f;
                    
                    vertex.x = float(x + dx);
                    vertex.y = float(y + dy);
                    
                    vertices_.push_back(vertex);
                    
                    vertices_.push_back(vertex);
                    vertices_.back().x += 1.0f;
                    
                    vertices_.push_back(vertex);
                    vertices_.back().x += 1.0f;
                    vertices_.back().y += 1.0f;
                    
                    vertices_.push_back(vertex);
                    vertices_.back().y += 1.0f;
                }
            }
        }

        verticesDirty_ = false;
    }

    void Sprite::updateBuffer() const
    {
        if (!bufferDirty_) {
            return;
        }

        if (bufferName_) {
            glDeleteBuffers(1, &bufferName_);
        }
        glGenBuffers(1, &bufferName_);

        bufferCount_ = GLsizei(vertices_.size());
        
        glBindBuffer(GL_ARRAY_BUFFER, bufferName_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * bufferCount_,
                     &vertices_[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        bufferDirty_ = false;
    }
}
