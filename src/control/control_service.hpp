#ifndef CRUST_CONTROL_SERVICE_HPP
#define CRUST_CONTROL_SERVICE_HPP

#include <vector>

namespace crust {
    class Game;
    class Task;
    
    class ControlService {
    public:
        typedef std::vector<Task *> TaskVector;
        
        explicit ControlService(Game *game);
        
        void addTask(Task *task);
        void removeTask(Task *task);
        
        void step(float dt);
        
    private:
        Game *game_;
        TaskVector tasks_;
    };
}

#endif
