#ifndef CRUST_BLOCK_RENDER_COMPONENT_HPP
#define CRUST_BLOCK_RENDER_COMPONENT_HPP

#include "render_component.hpp"

#include <memory>
#include <vector>
#include <SDL/SDL_opengl.h>

namespace crust {
    class Actor;
    class BlockPhysicsComponent;
    
    class BlockRenderComponent : public RenderComponent {
    public:
        // With 4 bytes per float, adds up to 32 bytes.
        struct DrawVertex {
            GLfloat red;
            GLfloat green;
            GLfloat blue;
            
            GLfloat normalX;
            GLfloat normalY;
            GLfloat normalZ;
            
            GLfloat x;
            GLfloat y;
        };
        
        explicit BlockRenderComponent(Actor *actor);

        void draw() const;
        
    private:
        Actor *actor_;
        BlockPhysicsComponent *physicsComponent_;

        float red_;
        float green_;
        float blue_;

        float normalX_;
        float normalY_;
        
        mutable std::vector<DrawVertex> quadDrawVertices_;
        mutable std::vector<DrawVertex> lineDrawVertices_;
        mutable bool drawVerticesDirty_;
        
        float getColorOffset(int x, int y, int i) const;
        std::size_t hashValue(std::size_t a) const;
        
        void updateDrawVertices() const;
        void updateQuadDrawVertices() const;
        void updateLineDrawVertices() const;
    };
}

#endif
