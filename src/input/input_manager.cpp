#include "input_manager.hpp"

#include "actor.hpp"
#include "config.hpp"
#include "convert.hpp"
#include "game.hpp"
#include "graphics_manager.hpp"
#include "monster_control_component.hpp"
#include "task.hpp"

#include <algorithm>

namespace crust {
    InputManager::InputManager(Game *game) :
        game_(game)
    { }
    
    void InputManager::addTask(Task *task)
    {
        tasks_.push_back(task);
    }
    
    void InputManager::removeTask(Task *task)
    {
        TaskVector::iterator i = std::find(tasks_.begin(), tasks_.end(), task);
        tasks_.erase(i);
    }
    
    void InputManager::step(float dt)
    {
        for (TaskVector::iterator i = tasks_.begin(); i != tasks_.end(); ++i) {
            (*i)->step(dt);
        }
        handleEvents();
        handleInput();
    }

    void InputManager::handleEvents()
    {
        SDL_Event event;
#if 1
        while (SDL_PollEvent(&event)) {
            handleEvent(&event);
        }
#else
        SDL_WaitEvent(&event);
        handleEvent(&event);
#endif
    }
    
    void InputManager::handleEvent(SDL_Event *event)
    {
        switch (event->type) {
            case SDL_QUIT:
                game_->setQuitting(true);
                break;
                
            case SDL_KEYDOWN:
                handleKeyDownEvent(event);
                break;
                
            case SDL_KEYUP:
                handleKeyUpEvent(event);
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                handleMouseButtonDownEvent(event);
                break;
                
            case SDL_MOUSEBUTTONUP:
                handleMouseButtonUpEvent(event);
                break;
        }
    }
    
    void InputManager::handleKeyDownEvent(SDL_Event *event)
    {
        switch (event->key.keysym.sym) {
            case SDLK_ESCAPE:                
                game_->setQuitting(true);
                break;
                
            case SDLK_1:
                if (game_->getPlayerActor()) {
                    MonsterControlComponent *controlComponent = convert(game_->getPlayerActor()->getControlComponent());
                    controlComponent->setActionMode(MonsterControlComponent::MINE_MODE);
                }
                break;
                
            case SDLK_2:
                if (game_->getPlayerActor()) {
                    MonsterControlComponent *controlComponent = convert(game_->getPlayerActor()->getControlComponent());
                    controlComponent->setActionMode(MonsterControlComponent::DRAG_MODE);
                }
                break;
                
            case SDLK_3:
                if (game_->getPlayerActor()) {
                    MonsterControlComponent *controlComponent = convert(game_->getPlayerActor()->getControlComponent());
                    controlComponent->setActionMode(MonsterControlComponent::DROP_MODE);
                }
                break;
                
            case SDLK_PLUS:
            {
                float scale = game_->getGraphicsManager()->getCameraScale();
                scale *= game_->getConfig()->cameraZoom;
                if (scale < game_->getConfig()->maxCameraScale) {
                    game_->getGraphicsManager()->setCameraScale(scale);
                }
            }
                break;
                
            case SDLK_MINUS:
            {
                float scale = game_->getGraphicsManager()->getCameraScale();
                scale /= game_->getConfig()->cameraZoom;
                if (scale > game_->getConfig()->minCameraScale) {
                    game_->getGraphicsManager()->setCameraScale(scale);
                }
            }
                break;
                
            case SDLK_UP:
                // cameraPosition_.y += config_->cameraPan / cameraScale_;
                break;
                
            case SDLK_LEFT:
                // cameraPosition_.x -= config_->cameraPan / cameraScale_;
                break;
                
            case SDLK_RIGHT:
                // cameraPosition_.x += config_->cameraPan / cameraScale_;
                break;
                
            case SDLK_DOWN:
                // cameraPosition_.y -= config_->cameraPan / cameraScale_;
                break;
        }
    }
    
    void InputManager::handleKeyUpEvent(SDL_Event *event)
    { }
    
    void InputManager::handleMouseButtonDownEvent(SDL_Event *event)
    { }
    
    void InputManager::handleMouseButtonUpEvent(SDL_Event *event)
    { }
    
    void InputManager::handleInput()
    {
        if (game_->getPlayerActor()) {
            MonsterControlComponent *controlComponent = convert(game_->getPlayerActor()->getControlComponent());
            
            int x = 0;
            int y = 0;
            Uint8 mouseButtons = SDL_GetMouseState(&x, &y);
            Uint8 *keyboardState = SDL_GetKeyboardState(0);
            
            bool leftControl = bool(keyboardState[SDL_SCANCODE_A]);
            bool rightControl = bool(keyboardState[SDL_SCANCODE_D]);
            bool jumpControl = bool(keyboardState[SDL_SCANCODE_SPACE]);
            bool actionControl = bool(keyboardState[SDL_SCANCODE_LSHIFT] || (mouseButtons & SDL_BUTTON_LMASK));
            Vector2 targetPosition = game_->getGraphicsManager()->getWorldPosition(Vector2(float(x), float(y)));
            
            controlComponent->setLeftControl(leftControl);
            controlComponent->setRightControl(rightControl);
            controlComponent->setJumpControl(jumpControl);
            controlComponent->setActionControl(actionControl);
            controlComponent->setTargetPosition(targetPosition);
        }
    }
}
