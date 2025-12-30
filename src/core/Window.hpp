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
    [[nodiscard]] i32 width() const { return m_width; }
    [[nodiscard]] i32 height() const { return m_height; }
    [[nodiscard]] bool isValid() const { return m_window != nullptr; }

    void setTitle(std::string_view title);

private:
    struct WindowDeleter {
        void operator()(SDL_Window* window) const {
            if (window) {
                SDL_DestroyWindow(window);
            }
        }
    };

    std::unique_ptr<SDL_Window, WindowDeleter> m_window;
    i32 m_width;
    i32 m_height;
};

}  // namespace pacman
