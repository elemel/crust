#include "camera.hpp"

#include <SDL/SDL_opengl.h>

namespace crust {
    Vector2 Camera::getWorldPoint(Vector2 const &screenPoint) const
    {
        Vector2 screenOffset = screenPoint - viewport_.getCenter();
        float scaledScreenHeight = 0.5f * scale_ * viewport_.getHeight();
        return position_ + Vector2(screenOffset.x, -screenOffset.y) / scaledScreenHeight;
    }
    
    void Camera::setProjection()
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(frustum_.p1.x, frustum_.p2.x, frustum_.p1.y, frustum_.p2.y,
                -1.0f, 1.0f);
        glMatrixMode(GL_MODELVIEW);
    }
}
