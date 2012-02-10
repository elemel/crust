#include "block_render_component.hpp"

#include "actor.hpp"
#include "block_physics_component.hpp"
#include "game.hpp"
#include "grid.hpp"
#include "wire.hpp"

namespace crust {
    BlockRenderComponent::BlockRenderComponent(Actor *actor) :
        actor_(actor),
        physicsComponent_(wire(actor->getPhysicsComponent())),

        red_(1.0f),
        green_(1.0f),
        blue_(1.0f),
    
        normalX_(0.0f),
        normalY_(0.0f),
    
        drawVerticesDirty_(true)
    {
        // float brightness = 0.5f + 0.3f * block->getGame()->getRandomFloat();
        red_ = 0.3f + 0.1f * actor->getGame()->getRandomFloat();
        green_ = 0.5f + 0.1f * actor->getGame()->getRandomFloat();
        blue_ = 0.7f + 0.1f * actor->getGame()->getRandomFloat();

        normalX_ = -0.05f + 0.1f * actor->getGame()->getRandomFloat();
        normalY_ = -0.05f + 0.1f * actor->getGame()->getRandomFloat();
    }
    
    void BlockRenderComponent::draw() const
    {
        updateDrawVertices();
        
        Vector2 position = physicsComponent_->getPosition();
        float angle = physicsComponent_->getAngle();
        glPushAttrib(GL_CURRENT_BIT);
        glPushMatrix();
        glTranslatef(position.x, position.y, 0.0f);
        glRotatef(angle * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);
        glScalef(0.1f, 0.1f, 1.0f);
        glTranslatef(-0.5f, -0.5f, 0.0f);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        
        if (!quadDrawVertices_.empty()) {
            glColorPointer(3, GL_FLOAT, sizeof(DrawVertex), &quadDrawVertices_[0].red);
            glNormalPointer(GL_FLOAT, sizeof(DrawVertex), &quadDrawVertices_[0].normalX);
            glVertexPointer(2, GL_FLOAT, sizeof(DrawVertex), &quadDrawVertices_[0].x);
            glDrawArrays(GL_QUADS, 0, GLsizei(quadDrawVertices_.size()));
        }
        
        if (!lineDrawVertices_.empty()) {
            glColorPointer(3, GL_FLOAT, sizeof(DrawVertex), &lineDrawVertices_[0].red);
            glNormalPointer(GL_FLOAT, sizeof(DrawVertex), &lineDrawVertices_[0].normalX);
            glVertexPointer(2, GL_FLOAT, sizeof(DrawVertex), &lineDrawVertices_[0].x);
            glDrawArrays(GL_LINES, 0, GLsizei(lineDrawVertices_.size()));
        }
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glPopMatrix();
        glPopAttrib();
    }

    float BlockRenderComponent::getColorOffset(int x, int y, int i) const
    {
        std::size_t a = hashValue((x << 16) + (y << 8) + i);
        return 0.1f * 0.001f * float(a % 1000);
    }
    
    // http://burtleburtle.net/bob/hash/integer.html
    std::size_t BlockRenderComponent::hashValue(std::size_t a) const
    {
        a -= (a << 6);
        a ^= (a >> 17);
        a -= (a << 9);
        a ^= (a << 4);
        a -= (a << 3);
        a ^= (a << 10);
        a ^= (a >> 15);
        return a;
    }
    
    void BlockRenderComponent::updateDrawVertices() const
    {
        if (drawVerticesDirty_) {
            updateQuadDrawVertices();
            updateLineDrawVertices();
            drawVerticesDirty_ = false;
        }
    }
    
    void BlockRenderComponent::updateQuadDrawVertices() const
    {
        Grid<unsigned char> const &grid = physicsComponent_->getGrid();

        quadDrawVertices_.clear();
        for (int y = 0; y < grid.getHeight(); ++y) {
            for (int x = 0; x < grid.getWidth(); ++x) {
                int type = grid.getElement(x + grid.getX(), y + grid.getY());
                if (type) {
                    DrawVertex vertex;
                    vertex.red = red_ + getColorOffset(x + grid.getX(), y + grid.getY(), 0);
                    vertex.green = green_ + getColorOffset(x + grid.getX(), y + grid.getY(), 1);
                    vertex.blue = blue_ + getColorOffset(x + grid.getX(), y + grid.getY(), 2);
                    
                    vertex.normalX = normalX_ - 0.025f + 0.5f * getColorOffset(x + grid.getX(), y + grid.getY(), 3);
                    vertex.normalY = normalY_ - 0.025f + 0.5f * getColorOffset(x + grid.getX(), y + grid.getY(), 4);
                    vertex.normalZ = 1.0f;
                    
                    vertex.x = float(x + grid.getX());
                    vertex.y = float(y + grid.getY());
                    
                    quadDrawVertices_.push_back(vertex);
                    
                    quadDrawVertices_.push_back(vertex);
                    quadDrawVertices_.back().x += 1;
                    
                    quadDrawVertices_.push_back(vertex);
                    quadDrawVertices_.back().x += 1;
                    quadDrawVertices_.back().y += 1;
                    
                    quadDrawVertices_.push_back(vertex);
                    quadDrawVertices_.back().y += 1;
                }
            }
        }
    }
    
    void BlockRenderComponent::updateLineDrawVertices() const
    {
        Grid<unsigned char> const &grid = physicsComponent_->getGrid();
        lineDrawVertices_.clear();
        
        DrawVertex vertex;
        vertex.red = 0.0f;
        vertex.green = 0.0f;
        vertex.blue = 0.0f;
        vertex.normalX = 0.0f;
        vertex.normalY = 0.0f;
        vertex.normalZ = 1.0f;
        vertex.x = 0.0f;
        vertex.y = 0.0f;
        
        for (int y = 0; y < grid.getHeight() + 1; ++y) {
            for (int x = 0; x < grid.getWidth() + 1; ++x) {
                bool center = (grid.getElement(x + grid.getX(), y + grid.getY())) != 0;
                bool left = (grid.getElement(x + grid.getX() - 1, y + grid.getY())) != 0;
                bool bottom = (grid.getElement(x + grid.getX(), y + grid.getY() - 1)) != 0;
                if (center != left) {
                    lineDrawVertices_.push_back(vertex);
                    lineDrawVertices_.back().x = float(x + grid.getX());
                    lineDrawVertices_.back().y = float(y + grid.getY());
                    lineDrawVertices_.push_back(vertex);
                    lineDrawVertices_.back().x = float(x + grid.getX());
                    lineDrawVertices_.back().y = float(y + grid.getY() + 1);
                }
                if (center != bottom) {
                    lineDrawVertices_.push_back(vertex);
                    lineDrawVertices_.back().x = float(x + grid.getX());
                    lineDrawVertices_.back().y = float(y + grid.getY());
                    lineDrawVertices_.push_back(vertex);
                    lineDrawVertices_.back().x = float(x + grid.getX() + 1);
                    lineDrawVertices_.back().y = float(y + grid.getY());
                }
            }
        }
    }
}
