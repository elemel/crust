#ifndef CRUST_STATE_HPP
#define CRUST_STATE_HPP

#include <memory>

namespace crust {
    class Task;

    class State {
    public:
        virtual ~State()
        { }

        virtual void create() = 0;
        virtual void destroy() = 0;

        virtual std::auto_ptr<State> transition() = 0;

        virtual Task *getTask() = 0;
        virtual Task const *getTask() const = 0;
    };
}

#endif
