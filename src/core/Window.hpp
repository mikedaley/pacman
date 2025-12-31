#pragma once

#include "utils/Types.hpp"

#include <SDL3/SDL.h>

#include <memory>
#include <string>
#include <string_view>

namespace pacman {

class Window {
public:
    Window(std::string_view title, i32 width, i32 height);
    ~Window() = default;

    // Non-copyable
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // Movable
    Window(Window&&) noexcept = default;
    Window& operator=(Window&&) noexcept = default;

    [[nodiscard]] SDL_Window* get() const { return m_window.get(); }

private:
    struct WindowDeleter {
        void operator()(SDL_Window* window) const {
            if (window) {
                SDL_DestroyWindow(window);
            }
        }
    };

    std::unique_ptr<SDL_Window, WindowDeleter> m_window;
};

}  // namespace pacman
