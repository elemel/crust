#ifndef CRUST_CAMERA_HPP
#define CRUST_CAMERA_HPP

#include "geometry.hpp"
#include "math.hpp"

namespace crust {
    class Camera {
    public:
        Camera() :
            scale_(0.0f)
        { }

        Vector2 const &getPosition() const
        {
            return position_;
        }
        
        void setPosition(Vector2 const &position)
        {
            position_ = position;
        }
        
        float getScale() const
        {
            return scale_;
        }
        
        void setScale(float scale)
        {
            scale_ = scale;
        }

        Box2 const &getViewport() const
        {
            return viewport_;
        }
        
        void setViewport(Box2 const &viewport)
        {
            viewport_ = viewport;
        }

        Vector2 getWorldPoint(Vector2 const &screenPoint) const;

        void setProjection();

    private:
        Vector2 position_;
        float scale_;
        Box2 viewport_;

        Matrix3 screenToWorld_;
        Matrix3 worldToScreen_;
        
        bool frustumDirty_;
        Box2 frustum_;

        void updateFrustum();
    };
}

#endif
