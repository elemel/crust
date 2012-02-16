#ifndef CRUST_CONTROL_COMPONENT_HPP
#define CRUST_CONTROL_COMPONENT_HPP

namespace crust {
    class Vector2;

    class ControlComponent {
    public:
        virtual ~ControlComponent()
        { }

        virtual void create() = 0;
        virtual void destroy() = 0;
        
        virtual bool getLeftControl() const = 0;
        virtual void setLeftControl(bool control) = 0;

        virtual bool getRightControl() const = 0;
        virtual void setRightControl(bool control) = 0;
        
        virtual bool getJumpControl() const = 0;
        virtual void setJumpControl(bool control) = 0;

        virtual bool getActionControl() const = 0;
        virtual void setActionControl(bool control) = 0;

        virtual Vector2 const &getTargetPosition() const = 0;
        virtual void setTargetPosition(Vector2 const &position) = 0;

        virtual void step(float dt) = 0;
    };
}

#endif
