#include "core/Window.hpp"

#include <SDL3/SDL.h>

#include <stdexcept>

namespace pacman {

Window::Window(std::string_view title, i32 width, i32 height) : m_window(nullptr) {
    SDL_Window* window = SDL_CreateWindow(std::string(title).c_str(), width, height, 0);

    if (!window) {
        throw std::runtime_error(std::string("Failed to create window: ") + SDL_GetError());
    }

    m_window.reset(window);
}

}  // namespace pacman
