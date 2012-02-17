#include "animation_service.hpp"

#include "task.hpp"

#include <algorithm>

namespace crust {
    AnimationService::AnimationService(Game *game) :
        game_(game)
    { }

    void AnimationService::addTask(Task *task)
    {
        tasks_.push_back(task);
    }

    void AnimationService::removeTask(Task *task)
    {
        TaskVector::iterator i = std::find(tasks_.begin(), tasks_.end(), task);
        tasks_.erase(i);
    }
    
    void AnimationService::step(float dt)
    {
        for (TaskVector::iterator i = tasks_.begin(); i != tasks_.end(); ++i) {
            (*i)->step(dt);
        }
    }
}
