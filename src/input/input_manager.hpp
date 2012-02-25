#ifndef CRUST_INPUT_MANAGER_HPP
#define CRUST_INPUT_MANAGER_HPP

#include <vector>
#include <SDL/SDL.h>

namespace crust {
    class Game;
    class Task;
    
    class InputManager {
    public:
        typedef std::vector<Task *> TaskVector;
        
        explicit InputManager(Game *game);
        
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
