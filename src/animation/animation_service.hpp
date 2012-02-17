#ifndef CRUST_ANIMATION_SERVICE_HPP
#define CRUST_ANIMATION_SERVICE_HPP

#include <vector>

namespace crust {
    class Game;
    class Task;

    class AnimationService {
    public:
        typedef std::vector<Task *> TaskVector;

        explicit AnimationService(Game *game);
        
        void addTask(Task *task);
        void removeTask(Task *task);

        void step(float dt);
        
    private:
        Game *game_;
        TaskVector tasks_;
    };
}

#endif
