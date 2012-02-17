#include "control_service.hpp"

#include "task.hpp"

#include <algorithm>

namespace crust {
    ControlService::ControlService(Game *game) :
        game_(game)
    { }
    
    void ControlService::addTask(Task *task)
    {
        tasks_.push_back(task);
    }
    
    void ControlService::removeTask(Task *task)
    {
        TaskVector::iterator i = std::find(tasks_.begin(), tasks_.end(), task);
        tasks_.erase(i);
    }
    
    void ControlService::step(float dt)
    {
        for (TaskVector::iterator i = tasks_.begin(); i != tasks_.end(); ++i) {
            (*i)->step(dt);
        }
    }
}
