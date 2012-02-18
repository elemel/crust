#ifndef CRUST_INPUT_SERVICE_HPP
#define CRUST_INPUT_SERVICE_HPP

#include <vector>
#include <SDL/SDL.h>

namespace crust {
    class Game;
    class Task;
    
    class InputService {
    public:
        typedef std::vector<Task *> TaskVector;
        
        explicit InputService(Game *game);
        
        void addTask(Task *task);
        void removeTask(Task *task);
        
        void step(float dt);
        
    private:
        Game *game_;
        TaskVector tasks_;

        void handleEvents();
        void handleEvent(SDL_Event *event);
        void handleKeyDownEvent(SDL_Event *event);
        void handleKeyUpEvent(SDL_Event *event);
        void handleMouseButtonDownEvent(SDL_Event *event);
        void handleMouseButtonUpEvent(SDL_Event *event);
        void handleInput();
    };
}

#endif
