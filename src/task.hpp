#ifndef CRUST_TASK_HPP
#define CRUST_TASK_HPP

namespace crust {
    class Task {
    public:
        virtual ~Task()
        { }

        virtual void step(float dt) = 0;
    };
}

#endif
